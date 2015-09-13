#include "logger.hpp"

using Logger::LoggingHandler;
using Logger::logInfo;
using Logger::logDebug;
using Logger::logWarn;
using Logger::logError;


void threadTest(int idx) {
    logInfo("Thread: " + std::to_string(idx));
}


int main(void) {
    LoggingHandler.init();
    std::thread threads[30];
    for (int idx = 0; idx < 30; ++ idx)
        threads[idx] = std::thread(threadTest, idx);

    for(auto& thread : threads)
        thread.join();

    return 0;
}
