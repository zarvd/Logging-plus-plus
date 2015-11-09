#include "../include/Logger.hpp"
#include <functional>
#include <iostream>

using Logger::Level::TRACE;
using Logger::Level::INFO;
using Logger::Level::DEBUG;
using Logger::Level::ERROR;
using Logger::Level::WARN;
auto & logger = Logger::LogHandler::getHandler();

using testFunc = std::function<void()>;

void
mulitThread(const int & thread) {
    for(unsigned i = 0; i < 200000; ++i) {
        Log(INFO) << "Log test thread" << thread;
    }
}

void
multiThreadTest() {
    const unsigned threadCount = 10;
    std::thread threads[threadCount];
    for(unsigned idx = 0; idx < threadCount; ++idx) {
        threads[idx] = std::thread(mulitThread, idx);
    }

    for(unsigned idx = 0; idx < threadCount; ++idx) {
        threads[idx].join();
    }
}

void
singleThreadTest() {
    const unsigned long msgCount = 2000000;
    for(unsigned idx = 0; idx < msgCount; ++idx) {
        Log(INFO) << "Single thread log test" << idx;
    }
}

void
countRunTime(const std::string & testName, testFunc func) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    {
        try {
            func();
        } catch(const std::exception & e) {
            std::cout << e.what() << std::endl;
        }
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << testName << " Run Time: " << elapsed_seconds.count()
              << std::endl;
}

void
testLevel() {
    Log(TRACE) << "test TRACE";
    Log(DEBUG) << "test DEBUG";
    Log(INFO) << "test INFO";
    Log(WARN) << "test WARN";
    Log(ERROR) << "test ERROR";
}

int
main(void) {
    // LoggingHandler.setOutput(Logger::Output::FILE, false);
    logger.setOutput(Logger::LogHandler::Output::CONSOLE, false);
    logger.setLogFile("multi.log");
    // LoggingHandler.setLogFile("single.log");
    logger.setLogLevel(TRACE);
    logger.init();
    // testLevel();
    countRunTime("multi", &multiThreadTest);
    countRunTime("single", &singleThreadTest);
    return 0;
}
