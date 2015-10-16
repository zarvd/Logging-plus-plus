#include <logger/Logger.hpp>  // install logger

using Logger::logger;
using Logger::Level::TRACE;
using Logger::Level::INFO;
using Logger::Level::DEBUG;
using Logger::Level::ERROR;
using Logger::Level::WARN;
using Output = Logger::LogHandler::Output;


int main(void) {
    logger.setOutput(Output::FILE, false);
    logger.init();

    Log(INFO) << "TEST Logging";
    return 0;
}
