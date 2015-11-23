#include <iostream>
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>
#include "../include/log_handler.h"

namespace logger {

LogHandler::LogHandler()
    : kMaxMsgSize(300),
      is_output_ready_(false),
      is_close_output_(false),
      is_stop_(true),
      output_thread_(),
      max_buffer_size_(50),
      flush_frequency_(3),
      log_dir_(""),
      log_file_("app.log"),
      log_level_(LogLevel::INFO),
      output_({{Output::FILE, true}, {Output::CONSOLE, true}}),
      log_read_buffer_(),
      log_write_buffer_() {}

LogHandler::~LogHandler() {
  std::unique_lock<std::mutex> output_lock(output_mtx_);

  while (!is_output_ready_) {
    output_cv_.wait(output_lock);
  }
  is_close_output_ = true;

  output_thread_.join();

  if (log_stream_.is_open()) {
    log_stream_.close();
  }
}

struct LogHandler::OutputEntity {
  std::string color;
  std::string logMsg;
};

/**
 * Before using a logger, you need to initialize it.
 * it will open a file Stream if it is allowed to write to a log file
 */
void LogHandler::Init() {
  output_thread_ = std::thread(&LogHandler::StartOutputThread, this);

  std::lock_guard<std::mutex> log_lock(log_mtx_);
  is_stop_ = false;
  FreshCurrentTime();  // fresh time before logging

  if (output_.at(Output::FILE)) {
    OpenLogStream();
  }
}

/**
 * Setting output filter
 */
void LogHandler::set_output(const Output& output, bool is_allowed) {
  std::lock_guard<std::mutex> lck(log_mtx_);
  if (!is_stop_) return;  // unable to modify when running
  switch (output) {
    case Output::FILE:
      if (!is_allowed && log_stream_.is_open()) {
        log_stream_.close();
      }
      this->output_.at(Output::FILE) = is_allowed;
      break;
    case Output::CONSOLE:
      this->output_.at(Output::CONSOLE) = is_allowed;
      break;
  }
}

/**
 * Setting log file and path, and it will set isWriteToFile to true
 */
void LogHandler::set_log_file(const std::string& log_path) {
  std::lock_guard<std::mutex> lock(log_mtx_);
  if (!is_stop_) return;

  if (log_stream_.is_open()) {
    log_stream_.close();
  }

  output_.at(Output::FILE) = true;

  PathToFile(log_path, log_dir_, log_file_);
}

/**
 * Setting log level
 */
void LogHandler::set_log_level(const LogLevel& level) {
  std::lock_guard<std::mutex> lock(log_mtx_);
  if (!is_stop_) return;

  log_level_ = level;
}

void LogHandler::set_flush_frequency(const unsigned fre) {
  // FIXME more precisely
  std::lock_guard<std::mutex> lock(log_mtx_);
  if (!is_stop_) return;

  flush_frequency_ = std::chrono::seconds(fre);
}

void LogHandler::set_max_buffer_size(const unsigned size) {
  std::lock_guard<std::mutex> lock(log_mtx_);
  if (!is_stop_) return;

  max_buffer_size_ = size;
}

/**
 * Log operation
 */
void LogHandler::Log(const LogLevel& level, const std::string& msg,
                     const std::string& file, const std::string& func,
                     const unsigned line) {
  if (is_stop_ || level < log_level_) return;

  const auto output = FormatOutput(level, msg, file, func, line);

  // it may block
  std::lock_guard<std::mutex> log_lock(log_mtx_);

  if (is_stop_) {
    throw std::logic_error("logging handler haven't been inited");
  }

  log_read_buffer_.push_back(output);

  // notify output thread to output
  if (log_read_buffer_.size() >= max_buffer_size_) {
    log_cv_.notify_one();
  }
}

/**
 * Open a file stream
 */
void LogHandler::OpenLogStream() const {
  if (log_stream_.is_open()) {
    log_stream_.close();
  }

  // test log directory and create directory if neccesary
  if (access(log_dir_.c_str(), F_OK) != 0 ||
      access(log_dir_.c_str(), W_OK) != 0) {
    std::string dir;
    for (std::size_t idx = 0; idx < log_dir_.length(); ++idx) {
      // create new directory recusively
      const char& curChar = log_dir_[idx];

      // get current directory
      if (curChar == '/') {
        dir = log_dir_.substr(0, idx);  // get new directory path
      } else if (idx + 1 == log_dir_.length()) {
        dir = log_dir_;
      } else {
        continue;
      }

      struct stat fileStat;
      if (stat(dir.c_str(), &fileStat) < 0) {
        if (mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
          throw std::runtime_error("Cannot create directory");
        }
      }
      if (!S_ISDIR(fileStat.st_mode)) {
        throw std::runtime_error("Directory error");
      }
    }
  }
  log_stream_.open(DirAndFileToPath(log_dir_, log_file_),
                   std::ofstream::out | std::ofstream::app);
}

void LogHandler::FreshCurrentTime() {
  std::time_t now;
  std::time(&now);
  current_time_ = std::ctime(&now);
  current_time_.pop_back();
}

/**
 * Another thread for output to file
 */
void LogHandler::StartOutputThread() {
  while (true) {
    if (!is_output_ready_) {
      // make sure engine is up
      std::lock_guard<std::mutex> lock(output_mtx_);  // protect isEngineReady
      is_output_ready_ = true;
      output_cv_.notify_one();
    }

    {
      // get write buffer
      std::unique_lock<std::mutex> logLck(
          log_mtx_);  // protect log_read_buffer_
      while (log_write_buffer_.empty()) {
        log_cv_.wait_for(logLck, flush_frequency_);
        log_write_buffer_.swap(log_read_buffer_);

        // close output thread
        if (is_close_output_ && log_write_buffer_.empty()) exit(0);

        // fresh time
        FreshCurrentTime();
      }
    }

    std::string toConsole;
    std::string toFile;
    for (const auto& logMsg : log_write_buffer_) {
      if (output_.at(Output::CONSOLE)) {
        toConsole += logMsg.color + logMsg.logMsg;
      }

      if (output_.at(Output::FILE)) {
        toFile += logMsg.logMsg;
      }
    }
    log_write_buffer_.clear();
    if (output_.at(Output::CONSOLE)) {
      OutputToConsole(toConsole);
    }

    if (output_.at(Output::FILE)) {
      OutputToFile(toFile);
      log_stream_ << std::flush;
    }
  }
}

/**
 * Print log to console
 */
void LogHandler::OutputToConsole(const std::string& logMsg) const {
  std::cout << logMsg;
}

/**
 * Print log to log file
 */
void LogHandler::OutputToFile(const std::string& logMsg) const {
  log_stream_ << logMsg;
}

/**
 * get formatted output log
 */
LogHandler::OutputEntity LogHandler::FormatOutput(const LogLevel& level,
                                                  const std::string& msg,
                                                  const std::string& file,
                                                  const std::string& func,
                                                  const unsigned line) const {
  std::string color;
  switch (level) {
    case LogLevel::TRACE:
      color = "\x1b[35m";  // magenta
      break;
    case LogLevel::DEBUG:
      color = "\x1b[34m";  // blue
      break;
    case LogLevel::INFO:
      color = "\x1b[32m";  // green
      break;
    case LogLevel::WARN:
      color = "\x1b[33m";  // yellow
      break;
    case LogLevel::ERROR:
      color = "\x1b[31m";  // red
      break;
  }
  char buffer[kMaxMsgSize];
  snprintf(buffer, kMaxMsgSize, "%s -> [%s::%s::%u] %s >> %s\n",
           GetLogLevel(level).c_str(), file.c_str(), func.c_str(), line,
           current_time_.c_str(), msg.c_str());
  OutputEntity output;
  output.logMsg = buffer;
  output.color = color;
  return output;
}
}
