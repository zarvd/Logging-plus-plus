#ifndef LOGGER_H
#define LOGGER_H

#include "LogStream.hpp"

namespace Logger {
LogHandler& logger = LogHandler::getHandler();  // Global logging handler
}

#define Log(level)                                              \
    if( ! Logger::LogHandler::isLevelAvailable(level)) ;        \
    else Logger::LogStream(level, __FILE__, __func__, __LINE__)

#endif /* LOGGER_H */
