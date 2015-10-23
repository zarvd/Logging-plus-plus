#include "../include/LogStream.hpp"


namespace Logger {
LogStream::LogStream(const Level & level, const std::string & file, const std::string & func, const unsigned line) :
    logHandler(LogHandler::getHandler()),
    logLevel(level),
    filename(file),
    funcname(func),
    line(line) {}

LogStream::~LogStream() {
    logHandler.log(logLevel, logMsg, filename, funcname, line);
}

LogStream &
LogStream::operator<<(const std::string & msg) {
    logMsg += msg;
    return *this;
}

LogStream &
LogStream::operator<<(const char * msg) {
    logMsg += msg;
    return *this;
}
}
