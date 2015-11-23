#ifndef LOGGING_PLUS_PLUS_LOG_STREAM_H_
#define LOGGING_PLUS_PLUS_LOG_STREAM_H_

#include "log_handler.h"

namespace logger {
/**
 * Log Stream
 */
class LogStream {
 public:
  LogStream(const LogLevel &, const std::string &, const std::string &,
            const unsigned);
  LogStream(const LogStream &) = delete;
  LogStream &operator=(const LogStream &) = delete;
  ~LogStream();

  template <typename T>
  LogStream &operator<<(const T &msg) {
    log_msg_ += std::to_string(msg);
    return *this;
  }
  LogStream &operator<<(const std::string &);
  LogStream &operator<<(const char *);
  LogStream &operator<<(char *);

 private:
  LogHandler &log_handler_;
  LogLevel log_level_;
  std::string filename;
  std::string funcname;
  unsigned line;
  std::string log_msg_;
};
}

#endif /* LOGGING_PLUS_PLUS_LOG_STREAM_H_ */
