#include "logger.hpp"

using Logger::LoggingHandler;
using Logger::logInfo;
using Logger::logDebug;
using Logger::logWarn;
using Logger::logError;
using logLevel = Logger::Level;

typedef void (*testFunc)();

void mulitThread(unsigned idx) {
    for(unsigned i = 1; i < 2000; ++ i) {
        log(logLevel::Info)("Thread: " + std::to_string(idx));
    }
}

void multiThreadTest() {
    const unsigned threadCount = 500;
    std::thread threads[threadCount];
    for(unsigned idx = 0; idx < threadCount; ++ idx) {
        threads[idx] = std::thread(mulitThread, idx);
    }

    for(auto& thread : threads)
        thread.join();
}

void singleThreadTest() {
    const unsigned long msgCount = 1000000;
    for(unsigned idx = 0; idx < msgCount; ++ idx) {
        log(logLevel::Info)("Log test");
    }
}

void whiteBox() {
    for(unsigned idx = 0; idx < 11; ++ idx) {
        log(logLevel::Warn)("Warning Test");
        log(logLevel::Error)("Test" + std::to_string(idx));
    }
}

void countRunTime(const std::string& testName, testFunc func) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    {
        func();
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << testName
              << " Run Time: "
              << elapsed_seconds.count()
              << std::endl;
}

void testCreateLog() {
    LoggingHandler.setLogFile("./log/log/log.log");
    LoggingHandler.init();
    log(logLevel::Info)("log");
}

int main(void) {
    LoggingHandler.setOutput(Logger::Output::CONSOLE, false);
    LoggingHandler.init();
    // countRunTime("multi", &multiThreadTest);
    countRunTime("single", &singleThreadTest);
    // countRunTime("whiteBox", &whiteBox);
    // logInfo("test");
    // whiteBox();
    return 0;
}
