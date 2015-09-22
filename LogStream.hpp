#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include "LogHandler.hpp"

namespace Logger {
    /**
     * Log Stream
     */
    class LogStream final {
    public:
        LogStream() = delete;
        LogStream(const Level&, const std::string&, const std::string&, const unsigned&);
        LogStream(const LogStream&) = delete;
        ~LogStream();

        template<typename T> LogStream& operator<<(const T& msg) {
            logMsg += std::to_string(msg);
            return *this;
        }
        LogStream& operator<<(const std::string&);
        LogStream& operator<<(const char *);

    private:
        LogHandler& logHandler;
        Level logLevel;
        std::string filename;
        std::string funcname;
        unsigned line;
        std::string logMsg;
    };
}

#endif /* LOGSTREAM_H */
