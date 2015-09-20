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

    struct LogEntity {
        unsigned long index;
        Level level;
        std::string time;
        std::string message;
        std::string filename;
        std::string funcname;
        unsigned line;
        std::string logMsg;  // NOTE
    };

    enum class Output {FILE, CONSOLE};

    enum class Input {FIN};

    class LogHandler final {
    public:
        LogHandler(const LogHandler&) = delete;
        ~LogHandler();

        void init();

        void setOutput(const Output&, const bool&);
        void setLogFile(const std::string&);
        void setLogLevel(const Level&);

        void log(const Level&, const std::string&, const std::string&, const std::string&, const unsigned&);
        bool isLevelAvailable(const Level&) const;
        static LogHandler& getHandler();

    private:
        LogHandler();
        // running status control
        std::mutex logMtx;
        std::mutex engineMtx;
        std::condition_variable logCV;
        std::condition_variable engineCV;
        bool isEngineReady;
        bool isCloseEngine;
        bool isStop;
        std::thread outputThread;

        // log configuration
        const unsigned MaxMsgSize;  // FIXME unkown buffer size
        unsigned maxBufferSize;
        std::chrono::seconds flushFrequency;
        std::string logDir;
        std::string logFile;
        std::ofstream logStream;
        alignas(64) std::string currentTime;
        unsigned long logCount;
        Level logLevel;
        std::map<Output, bool> output;

        // log buffer
        // NOTE avoid false sharing
        alignas(64) std::queue<std::shared_ptr<LogEntity> > logReadBuffer;
        alignas(64) std::queue<std::shared_ptr<LogEntity> > logWriteBuffer;

        // method
        void outputEngine();
        void openLogStream();
        void outputToConsole(const std::string&) const;
        void outputToFile(const std::string&);
        std::string formatOutput(std::shared_ptr<LogEntity>) const;
    };

    extern LogHandler& LoggingHandler;  // Global logging handler

    class LogStream final {
    public:
        LogStream() = delete;
        LogStream(const Level&, const bool&, const std::string&, const std::string&, const unsigned&);
        LogStream(const LogStream&) = default;
        ~LogStream();

        template<typename T>
            LogStream& operator<<(const T& msg) {
            logMsg += std::to_string(msg);
            return *this;
        }
        LogStream& operator<<(const std::string&);
        LogStream& operator<<(const char *);

        void operator<<(const Input&);

    private:
        bool isAvailable;
        Level logLevel;
        std::string filename;
        std::string funcname;
        unsigned line;
        std::string logMsg;
    };

    inline LogStream _log(const Level& level, const std::string& file, const std::string& func, const unsigned& line) {
        LogStream logStream(level, LoggingHandler.isLevelAvailable(level),
                            file, func, line);
        return logStream;
    }

#define Log(level)                                          \
    if( ! Logger::LoggingHandler.isLevelAvailable(level))   \
        ;                                                   \
    else                                                    \
        _log(level, __FILE__, __func__, __LINE__)
}

#endif /* LOGGER_H */
