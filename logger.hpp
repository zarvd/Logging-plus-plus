#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <cstdio>
#include <cstdarg>
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

    inline std::string getCurrentTime() {
        return getTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    }

    struct Log {
        unsigned long index;
        Level level;
        std::string time;
        std::string message;
        std::string logMsg;  // NOTE
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
        void log(const Level&, const char *, va_list);
        void setLogLevel(const Level&);

    private:
        std::mutex logMtx;
        std::condition_variable logCV;
        std::thread outputThread;
        bool isStop;

        const unsigned MaxMsgSize;  // FIXME unkown buffer size
        unsigned maxBufferSize;
        std::chrono::seconds flushFrequency;
        std::string logDir;
        std::string logFile;
        std::ofstream logStream;
        std::string currentTime;

        unsigned long logCount;
        Level logLevel;

        std::queue<std::shared_ptr<Log> > logReadBuffer;
        std::queue<std::shared_ptr<Log> > logWriteBuffer;

        std::map<Output, bool> output;

        void outputEngine();
        void openLogStream();
        void outputToConsole(const std::string&) const;
        void outputToFile(const std::string&);
        std::string formatOutput(std::shared_ptr<Log>) const;
    };

    extern LogHandler LoggingHandler;  // Global logging handler

    inline void logInfo(const char * fmt, ...) {
        va_list args;
        va_start(args, fmt);
        LoggingHandler.log(Level::Info, fmt, args);
        va_end(args);
    }

    inline void logDebug(const char * fmt, ...) {
        va_list args;
        va_start(args, fmt);
        LoggingHandler.log(Level::Debug, fmt, args);
        va_end(args);
    }

    inline void logWarn(const char * fmt, ...) {
        va_list args;
        va_start(args, fmt);
        LoggingHandler.log(Level::Warn, fmt, args);
        va_end(args);
    }

    inline void logError(const char * fmt, ...) {
        va_list args;
        va_start(args, fmt);
        LoggingHandler.log(Level::Error, fmt, args);
        va_end(args);
    }

    typedef void (*logFunc)(const char *, ...);

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
