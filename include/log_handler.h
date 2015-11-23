#ifndef LOGGING_PLUS_PLUS_LOG_HANDLER_H_
#define LOGGING_PLUS_PLUS_LOG_HANDLER_H_

#include <fstream>
#include <map>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "helper.h"

namespace logger {

enum class LogLevel { TRACE = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4 };

inline std::string GetLogLevel(const LogLevel &level) {
  switch (level) {
    case LogLevel::TRACE:
      return "TRACE";
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    default:
      throw;
  }
}

/**
 * Singleton class
 */
class LogHandler {
 public:
  LogHandler(const LogHandler &) = delete;
  LogHandler &operator=(const LogHandler &) = delete;
  ~LogHandler();

  struct OutputEntity;

  void Init();

  // configuration
  enum class Output { FILE, CONSOLE };
  void set_output(const Output &, const bool);
  void set_log_file(const std::string &);
  void set_log_level(const LogLevel &);
  void set_flush_frequency(const unsigned);
  void set_max_buffer_size(const unsigned);
  // main method
  void Log(const LogLevel &, const std::string &msg, const std::string &file,
           const std::string &func, const unsigned line);
  // other helpers
  static bool IsLevelAvailable(const LogLevel &level) {
    return level >= GetHandler().log_level_;
  }
  // return a static global log handler, singleton
  static LogHandler &GetHandler() {
    static LogHandler instance;
    return instance;
  }

  const unsigned kMaxMsgSize;  // Max single log msg size
 private:
  LogHandler();
  void StartOutputThread();
  void FreshCurrentTime();
  void OpenLogStream() const;
  void OutputToConsole(const std::string &) const;
  void OutputToFile(const std::string &) const;
  OutputEntity FormatOutput(const LogLevel &level, const std::string &msg,
                            const std::string &file, const std::string &func,
                            const unsigned line) const;

  // running status control
  mutable std::mutex log_mtx_;
  mutable std::mutex output_mtx_;
  std::condition_variable log_cv_;     // condition: logWriteBuffer
  std::condition_variable output_cv_;  // condition: isEngineReady
  bool is_output_ready_;
  bool is_close_output_;
  bool is_stop_;
  std::thread output_thread_;

  // log configuration
  unsigned max_buffer_size_;  // max logWriteBuffer
  std::chrono::seconds
      flush_frequency_;  // output engine flush buffer frequency
  std::string log_dir_;
  std::string log_file_;
  mutable std::ofstream log_stream_;
  std::string current_time_;
  LogLevel log_level_;             // limit log level
  std::map<Output, bool> output_;  // limit output

  // log buffer
  std::deque<OutputEntity> log_read_buffer_;
  std::deque<OutputEntity> log_write_buffer_;
};
}

#endif /* LOGGING_PLUS_PLUS_LOG_HANDLER_H_ */
