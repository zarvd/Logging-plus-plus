#include "../include/log_stream.h"

namespace logger {
LogStream::LogStream(const LogLevel& level, const std::string& file,
                     const std::string& func, const unsigned line)
    : log_handler_(LogHandler::GetHandler()),
      log_level_(level),
      filename(file),
      funcname(func),
      line(line) {}

LogStream::~LogStream() {
  log_handler_.Log(log_level_, log_msg_, filename, funcname, line);
}

LogStream& LogStream::operator<<(const std::string& msg) {
  log_msg_ += msg;
  return *this;
}

LogStream& LogStream::operator<<(const char* msg) {
  log_msg_ += msg;
  return *this;
}

LogStream& LogStream::operator<<(char* msg) {
  log_msg_ += msg;
  return *this;
}
}
