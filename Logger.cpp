#include "Logger.hpp"


namespace Logger {
LogHandler& LoggingHandler = LogHandler::getHandler();  // Global logging handler
}
