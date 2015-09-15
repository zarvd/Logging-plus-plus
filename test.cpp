#include "logger.hpp"

using Logger::LoggingHandler;
using logLevel = Logger::Level;


void log() {
}

void threadTest(int idx) {
    // logInfo();
    log(logLevel::Info)("Thread: " + std::to_string(idx));
}

int main(void) {
    LoggingHandler.init();
    // LoggingHandler.setLogLevel(Logger::Level::Error);
    std::thread threads[30];
    for (int idx = 0; idx < 30; ++ idx) {
        threads[idx] = std::thread(threadTest, idx);
    }


    for(auto& thread : threads)
        thread.join();

    return 0;
}
