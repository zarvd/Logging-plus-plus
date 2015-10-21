#include "../include/Logger.hpp"

auto & logger = Logger::LogHandler::getHandler();
using Logger::Level::TRACE;
using Logger::Level::INFO;
using Logger::Level::DEBUG;
using Logger::Level::ERROR;
using Logger::Level::WARN;
using Output = Logger::LogHandler::Output;


int main(void) {
    logger.init();
    Log(INFO) << "Hello, world!";
    return 0;
}
