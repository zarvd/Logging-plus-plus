#ifndef LOGGER_H
#define LOGGER_H

#include "LogStream.hpp"

#define Log(level)                                              \
    if( ! Logger::LogHandler::isLevelAvailable(level)) ;        \
    else Logger::LogStream(level, __FILE__, __func__, __LINE__)

#endif /* LOGGER_H */
