#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <sys/stat.h>
#include "lib.hpp"


namespace Logger {
    enum class Level {Info, Debug, Warn, Error};

    inline std::string getLogLevel(const Level& level) {
        switch(level) {
        case Level::Info:
            return "INFO";
        case Level::Debug:
            return "DEBUG";
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
        Level level;
        std::time_t time;
        std::string message;
    };


    class LogHandler {
    public:
        LogHandler();

        void init();
        void setWriteToFile(const bool&);
        void setLogFile(const std::string&);
        void log(const Level&, const std::string&);

    private:
        bool isWriteToFile;
        std::string logDir;
        std::string logFile;
        std::ofstream logStream;
        Log logMsg;

        void openLogStream();
        void printToConsole() const;
        void writeToFile();
    };

    extern std::mutex logMtx;
    extern LogHandler LoggingHandler;

    inline void log(Level level, const std::string& msg) {
        LoggingHandler.log(level, msg);
    }

    inline void logInfo(const std::string& msg) {
        log(Level::Info, msg);
    }

    inline void logDebug(const std::string& msg) {
        log(Level::Debug, msg);
    }

    inline void logWarn(const std::string& msg) {
        log(Level::Warn, msg);
    }

    inline void logError(const std::string& msg) {
        log(Level::Error, msg);
    }
}

#endif /* LOGGER_H */
