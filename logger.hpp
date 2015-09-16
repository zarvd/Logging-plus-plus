#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <sys/stat.h>
#include <exception>
#include <map>
#include <queue>
#include "lib.hpp"


namespace Logger {
    enum Level {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3
    };

    inline std::string getLogLevel(const Level& level) {
        switch(level) {
        case Level::Debug:
            return "DEBUG";
        case Level::Info:
            return "INFO";
        case Level::Warn:
            return "WARN";
        case Level::Error:
            return "ERROR";
        }
    }

    inline std::string getTime(const std::time_t rawTime) {
        std::string time = std::ctime(&rawTime);
        if(time.back() == '\n') {
            time.pop_back();
        }
        return time;
    }

    struct Log {
        unsigned long index;
        Level level;
        std::time_t time;
        std::string message;
    };

    enum class Output {FILE, CONSOLE};

    class LogHandler final {
    public:
        LogHandler();
        ~LogHandler();

        void init();
        void stop();
        void setOutput(const Output&, const bool&);
        void setLogFile(const std::string&);
        void log(const Level&, const std::string&);
        void setLogLevel(const Level&);

    private:
        std::mutex logMtx;
        std::condition_variable logCV;
        std::thread outputThread;
        bool isStop;

        unsigned MaxBufferSize;
        std::chrono::seconds flushFrequency;
        std::string logDir;
        std::string logFile;
        std::ofstream logStream;

        unsigned long logCount;
        Level logLevel;

        std::queue<std::shared_ptr<Log> > logReadBuffer;
        std::queue<std::shared_ptr<Log> > logWriteBuffer;

        std::map<Output, bool> output;

        void outputEngine();
        void openLogStream();
        void outputToConsole(std::shared_ptr<Log>) const;
        void outputToFile(std::shared_ptr<Log>);
        std::string formatOutput(std::shared_ptr<Log>) const;
    };

    extern LogHandler LoggingHandler;  // Global logging handler

    inline void logInfo(const std::string& msg) {
        LoggingHandler.log(Level::Info, msg);
    }

    inline void logDebug(const std::string& msg) {
        LoggingHandler.log(Level::Debug, msg);
    }

    inline void logWarn(const std::string& msg) {
        LoggingHandler.log(Level::Warn, msg);
    }

    inline void logError(const std::string& msg) {
        LoggingHandler.log(Level::Error, msg);
    }

    typedef void (*logFunc)(const std::string&);

    inline logFunc log(const Level& level) {
        switch(level) {
        case Level::Debug:
            return &logDebug;
        case Level::Info:
            return &logInfo;
        case Level::Warn:
            return &logWarn;
        case Level::Error:
            return &logError;
        }
    }
}

#endif /* LOGGER_H */
