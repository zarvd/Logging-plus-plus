#include "Logger.hpp"

using Logger::LoggingHandler;
using logLevel = Logger::Level;

typedef void (*testFunc)();

void mulitThread(const int& thread) {
    for(unsigned i = 0; i < 200000; ++ i) {
        Log(logLevel::Info) << "Log test thread" << thread;
    }
}

void multiThreadTest() {
    const unsigned threadCount = 10;
    std::thread threads[threadCount];
    for(unsigned idx = 0; idx < threadCount; ++ idx) {
        threads[idx] = std::thread(mulitThread, idx);
    }

    for(unsigned idx = 0; idx < threadCount; ++ idx) {
        threads[idx].join();
    }
}

void singleThreadTest() {
    const unsigned long msgCount = 2000000;
    for(unsigned idx = 0; idx < msgCount; ++ idx) {
        Log(logLevel::Info) << "Single thread log test" << idx;
    }
}

void countRunTime(const std::string& testName, testFunc func) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    {
        try {
            func();
        } catch(const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
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
    Log(logLevel::Info) << "test" << 1;
}

int main(void) {
    // LoggingHandler.setOutput(Logger::Output::FILE, false);
    LoggingHandler.setOutput(Logger::LogHandler::Output::CONSOLE, false);
    LoggingHandler.setLogFile("multi.log");
    // LoggingHandler.setLogFile("single.log");
    LoggingHandler.init();
    // Log(logLevel::Info) << "begin";
    // sleep(5);
    // Log(logLevel::Info) << "end";
    countRunTime("multi", &multiThreadTest);
    // countRunTime("single", &singleThreadTest);
    return 0;
}
