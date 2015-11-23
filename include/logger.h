#ifndef LOGGING_PLUS_PLUS_LOGGER_H_
#define LOGGING_PLUS_PLUS_LOGGER_H_

#include "log_stream.h"

#define Log(level)                                    \
  if (!::logger::LogHandler::IsLevelAvailable(level)) \
    ;                                                 \
  else                                                \
  ::logger::LogStream(level, __FILE__, __func__, __LINE__)

#endif /* LOGGING_PLUS_PLUS_LOGGER_H_ */
