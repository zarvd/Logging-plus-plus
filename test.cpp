#include "logger.hpp"


int main(void) {
    Logger::LoggingHandler.init();
    Logger::logInfo("Init");
    return 0;
}
