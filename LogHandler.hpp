#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include "LoggingLib.hpp"


namespace Logger {
    /**
     * Log handler
     */
    class LogHandler final {
    public:

        LogHandler(const LogHandler&) = delete;
        ~LogHandler();

        struct OutputEntity;

        void init();

        // configuration
        enum class Output {FILE, CONSOLE};
        void setOutput(const Output&, const bool&);
        void setLogFile(const std::string&);
        void setLogLevel(const Level&);
        void setFlushFrequency(const unsigned&);
        void setMaxBufferSize(const unsigned&);

        // main method
        void log(const Level&, const std::string&, const std::string&, const std::string&, const unsigned&);

        // other helpers
        static bool isLevelAvailable(const Level&);

        // return a static global log handler, singleton
        static LogHandler& getHandler();

    private:
        LogHandler();

        // running status control
        mutable std::mutex logMtx;
        mutable std::mutex engineMtx;
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
        mutable std::ofstream logStream;
        std::string currentTime;
        Level logLevel;  // limit log level
        std::map<Output, bool> output;  // limit output

        // log buffer
        // NOTE avoid false sharing
        std::deque<OutputEntity> logReadBuffer;
        std::deque<OutputEntity> logWriteBuffer;

        // method
        void outputEngine();
        void freshCurrentTime();
        void openLogStream() const;
        void outputToConsole(const std::string&) const;
        void outputToFile(const std::string&) const;
        OutputEntity formatOutput(const Level&, const std::string&,
                                  const std::string&, const std::string&,
                                  const unsigned&) const;
    };
}

#endif /* LOGHANDLER_H */
