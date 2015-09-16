#include "logger.hpp"

using Logger::LoggingHandler;
using logLevel = Logger::Level;

typedef void (*testFunc)();

void mulitThread(unsigned idx) {
    log(logLevel::Info)("Thread: " + std::to_string(idx));
}

void multiThreadTest() {
    const unsigned threadCount = 1000;
    std::thread threads[threadCount];
    for(unsigned idx = 0; idx < threadCount; ++ idx) {
        threads[idx] = std::thread(mulitThread, idx);
    }

    for(auto& thread : threads)
        thread.join();
}

void singleThreadTest() {
    const unsigned msgCount = 1000;
    for(unsigned idx = 0; idx < msgCount; ++ idx) {
        log(logLevel::Info)("Thread: " + std::to_string(idx));
    }
}

void countRunTime(const std::string& testName, testFunc func) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    {
        LoggingHandler.init();
        func();
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    std::cout << testName
              << " Run Time: "
              << elapsed_seconds.count()
              << std::endl;
}

int main(void) {
    countRunTime("multi", &multiThreadTest);
    // countRunTime("single", &singleThreadTest);
    return 0;
}
