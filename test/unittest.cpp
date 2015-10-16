#include "../include/Logger.hpp"

using Logger::logger;
using Logger::Level::TRACE;
using Logger::Level::INFO;
using Logger::Level::DEBUG;
using Logger::Level::ERROR;
using Logger::Level::WARN;
using Output = Logger::LogHandler::Output;


int main(void) {
    logger.init();
    return 0;
}
