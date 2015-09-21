#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#include <fstream>
#include <string>
#include <map>
#include <queue>

#include <ctime>
#include <chrono>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <exception>

#include <unistd.h>
#include <sys/stat.h>


namespace Logger {

    /**
     * Log msg level
     */
    enum Level {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3
    };

    /**
     * A single log msg entity
     */
    struct LogEntity {
        Level level;
        std::string time;
        std::string message;
        std::string filename;
        std::string funcname;
        unsigned line;
        std::string logMsg;  // NOTE
    };

    /**
     * Log handler
     */
    class LogHandler final {
    public:
        LogHandler(const LogHandler&) = delete;
        ~LogHandler();

        void init();

        // configuration
        enum class Output {FILE, CONSOLE};
        void setOutput(const Output&, const bool&);
        void setLogFile(const std::string&);
        void setLogLevel(const Level&);

        // main method
        void log(const Level&, const std::string&, const std::string&, const std::string&, const unsigned&);

        // other helpers
        bool isLevelAvailable(const Level&) const;

        // return a static global log handler, singleton
        static LogHandler& getHandler();

    private:
        LogHandler();

        // running status control
        std::mutex logMtx;
        std::mutex engineMtx;
        std::condition_variable logCV;  // condition: logWriteBuffer
        std::condition_variable engineCV;  // condition: isEngineReady
        bool isEngineReady;
        bool isCloseEngine;
        bool isStop;
        std::thread outputThread;

        // log configuration
        const unsigned MaxMsgSize;  // Max single log msg size
        unsigned maxBufferSize;  // max logWriteBuffer
        std::chrono::seconds flushFrequency;  // output engine flush buffer frequency
        std::string logDir;
        std::string logFile;
        std::ofstream logStream;
        std::string currentTime;
        Level logLevel;  // limit log level
        std::map<Output, bool> output;  // limit output

        // log buffer
        // NOTE avoid false sharing
        alignas(64) std::queue<std::shared_ptr<LogEntity> > logReadBuffer;
        alignas(64) std::queue<std::shared_ptr<LogEntity> > logWriteBuffer;

        // method
        void outputEngine();
        void freshCurrentTime();
        void openLogStream();
        void outputToConsole(const std::string&) const;
        void outputToFile(const std::string&);
        std::string formatOutput(std::shared_ptr<LogEntity>) const;
    };

    /**
     * Log Stream
     */
    class LogStream final {
    public:
        LogStream() = delete;
        LogStream(const Level&, const std::string&, const std::string&, const unsigned&);
        LogStream(const LogStream&) = delete;
        ~LogStream();

        template<typename T> LogStream& operator<<(const T& msg) {
            logMsg += std::to_string(msg);
            return *this;
        }
        LogStream& operator<<(const std::string&);
        LogStream& operator<<(const char *);

    private:
        Level logLevel;
        std::string filename;
        std::string funcname;
        unsigned line;
        std::string logMsg;
    };

    extern LogHandler& LoggingHandler;  // Global logging handler

    /**
     * Transfer Log Level to std::string
     */
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

    /**
     * Separate path into diretory and filename
     */
    inline void pathToFile(const std::string& path, std::string& dir, std::string& filename) {
        std::size_t foundPos = path.find_last_of("/");
        if(foundPos != std::string::npos) {
            dir = path.substr(0, foundPos);
            filename = path.substr(foundPos + 1);
        } else {
            dir = "";
            filename = path;
        }
    }

    /**
     * Combine diretory and filename
     */
    inline std::string dirAndFileToPath(const std::string& dir, const std::string& filename) {
        if(dir == "") {
            return "./" + filename;
        } else if(dir.back() == '/') {
            return dir + filename;
        } else {
            return dir + "/" + filename;
        }
    }
}

#define Log(level)                                              \
    if( ! Logger::LoggingHandler.isLevelAvailable(level)) ;     \
    else Logger::LogStream(level, __FILE__, __func__, __LINE__)

#endif /* LOGGER_H */
