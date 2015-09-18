#include "logger.hpp"

using Logger::LoggingHandler;
using Logger::logInfo;
using Logger::logDebug;
using Logger::logWarn;
using Logger::logError;
using Logger::Input;
using logLevel = Logger::Level;

typedef void (*testFunc)();

void mulitThread(int thread) {
    for(unsigned i = 1; i < 100000; ++ i) {
        const char * buf = "Thread";
        logInfo("Log test %s: %d", buf, thread);
        log(logLevel::Info) << "Log test " << buf << " " << thread << Input::FIN;
    }
}

void multiThreadTest() {
    const unsigned threadCount = 10;
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
        log(logLevel::Info) << "Log test " << Input::FIN;
    }
}

void whiteBox() {
    for(unsigned idx = 0; idx < 11; ++ idx) {
        log(logLevel::Info) << "Log test " << Input::FIN;
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
    log(logLevel::Info) << "test" << 1;
}

int main(void) {
    // LoggingHandler.setOutput(Logger::Output::CONSOLE, false);
    LoggingHandler.setOutput(Logger::Output::FILE, false);
    LoggingHandler.init();
    // const unsigned long msgCount = 10;
    // for(unsigned idx = 0; idx < msgCount; ++ idx) {
    //     log(logLevel::Info) << "Log test " << Input::FIN;
    // }
    log(logLevel::Info) << "Hello" << " Gallon" << 1 << 2.1243 << Input::FIN;
    // countRunTime("multi", &multiThreadTest);
    // countRunTime("single", &singleThreadTest);
    // countRunTime("whiteBox", &whiteBox);
    // logInfo("test");
    // whiteBox();
    return 0;
}
