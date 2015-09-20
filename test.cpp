#include "logger.hpp"

using Logger::LoggingHandler;
using Logger::Input;
using logLevel = Logger::Level;

typedef void (*testFunc)();

void mulitThread(int thread) {
    for(unsigned i = 0; i < 200000; ++ i) {
        Log(logLevel::Info) << "Log test " << "Thread" << thread << Input::FIN;
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
    const unsigned long msgCount = 2000000;
    for(unsigned idx = 0; idx < msgCount; ++ idx) {
        Log(logLevel::Info) << "Log test " << Input::FIN;
    }
}

void whiteBox() {
    for(unsigned idx = 0; idx < 11; ++ idx) {
        Log(logLevel::Info) << "Log test " << Input::FIN;
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
    Log(logLevel::Info) << "test" << 1;
}

int main(void) {
    LoggingHandler.setOutput(Logger::Output::CONSOLE, false);
    LoggingHandler.setLogFile("multi.log");
    // LoggingHandler.setLogFile("single.log");
    LoggingHandler.init();
    countRunTime("multi", &multiThreadTest);
    // countRunTime("single", &singleThreadTest);
    return 0;
}
