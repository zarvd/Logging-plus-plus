#include "LogHandler.hpp"

namespace Logger {
    LogHandler::LogHandler() :
        isEngineReady(false),
        isCloseEngine(false),
        isStop(true),
        outputThread(&LogHandler::outputEngine, this),
        MaxMsgSize(200),
        maxBufferSize(50),
        flushFrequency(3),
        logDir(""),
        logFile("app.log"),
        logLevel(Level::Info),
        output({{Output::FILE, true},
                {Output::CONSOLE, true}}),
        logReadBuffer(),
        logWriteBuffer() {

    }

    struct LogHandler::LogEntity {
        Level level;
        std::string time;
        std::string message;
        std::string filename;
        std::string funcname;
        unsigned line;
        std::string logMsg;  // NOTE
    };

    LogHandler::~LogHandler() {
        std::unique_lock<std::mutex> engineLck(engineMtx);

        while( ! isEngineReady) {
            engineCV.wait(engineLck);
        }
        isCloseEngine = true;

        outputThread.join();

        if(logStream.is_open()) {
            logStream.close();
        }
    }

    /**
     * Before using a logger, you need to initialize it.
     * it will open a file Stream if it is allowed to write to a log file
     */
    void LogHandler::init() {

        std::lock_guard<std::mutex> logLck(logMtx);
        isStop = false;
        freshCurrentTime();  // fresh time before logging

        if(output.at(Output::FILE)) {
            openLogStream();
        }
    }

    /**
     * Setting output filter
     */
    void LogHandler::setOutput(const Output& output, const bool& isAllowed) {
        std::lock_guard<std::mutex> lck(logMtx);
        if( ! isStop) return;  // unable to modify when running
        switch(output) {
        case Output::FILE:
            if( ! isAllowed && logStream.is_open()) {
                logStream.close();
            }
            this->output.at(Output::FILE) = isAllowed;
            break;
        case Output::CONSOLE:
            this->output.at(Output::CONSOLE) = isAllowed;
            break;
        }
    }

    /**
     * Setting log file and path, and it will set isWriteToFile to true
     */
    void LogHandler::setLogFile(const std::string& logPath) {
        std::lock_guard<std::mutex> lck(logMtx);
        if( ! isStop) return;

        if(logStream.is_open()) {
            logStream.close();
        }

        output.at(Output::FILE) = true;

        pathToFile(logPath, logDir, logFile);
    }

    /**
     * Setting log level
     */
    void LogHandler::setLogLevel(const Level& level) {
        std::lock_guard<std::mutex> lck(logMtx);
        if( ! isStop) return;

        logLevel = level;
    }

    void LogHandler::setFlushFrequency(const unsigned& fre) {
        std::lock_guard<std::mutex> lck(logMtx);
        if( ! isStop) return;

        flushFrequency = std::chrono::seconds(fre);
    }

    void LogHandler::setMaxBufferSize(const unsigned& size) {
        std::lock_guard<std::mutex> lck(logMtx);
        if( ! isStop) return;

        maxBufferSize = size;
    }

    bool LogHandler::isLevelAvailable(const Logger::Level& level) {
        return level >= getHandler().logLevel;
    }

    /**
     * Log operation
     */
    void LogHandler::log(const Level& level, const std::string& msg,
                         const std::string& file, const std::string& func,
                         const unsigned& line) {
        if(level < logLevel) return;

        std::shared_ptr<LogEntity> logMsg(new LogEntity);
        logMsg->time = currentTime;
        logMsg->level = level;
        logMsg->message = msg;
        logMsg->filename = file;
        logMsg->funcname = func;
        logMsg->line = line;
        logMsg->logMsg = formatOutput(logMsg);  // NOTE

        // it may block
        std::lock_guard<std::mutex> logLck(logMtx);

        if(isStop) {
            throw std::logic_error("logging handler haven't been inited");
        }

        logReadBuffer.push(logMsg);

        // notify output thread to output
        if(logReadBuffer.size() >= maxBufferSize) {
            logCV.notify_one();
        }
    }

    /**
     * Open a file stream
     */
    void LogHandler::openLogStream() {
        if(logStream.is_open()) {
            logStream.close();
        }

        // test log directory and create directory if neccesary
        if(access(logDir.c_str(), F_OK) != 0 || access(logDir.c_str(), W_OK) != 0) {
            std::string dir;
            for(unsigned short idx = 0; idx < logDir.length(); ++ idx) {
                // create new directory recusively
                const char& curChar = logDir[idx];

                // get current directory
                if(curChar == '/') {
                    dir = logDir.substr(0, idx);  // get new directory path
                } else if(idx + 1 == logDir.length()) {
                    dir = logDir;
                } else continue;

                struct stat fileStat;
                if(stat(dir.c_str(), &fileStat) < 0) {
                    if(mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                        std::cout << dir << std::endl;
                        throw std::runtime_error("Cannot create directory");
                    }
                }
                if( ! S_ISDIR(fileStat.st_mode)) {
                    throw std::runtime_error("Directory error");
                }

            }
        }
        logStream.open(dirAndFileToPath(logDir, logFile), std::ofstream::out | std::ofstream::app);
    }

    void LogHandler::freshCurrentTime() {
        std::time_t now;
        std::time(&now);
        currentTime = std::ctime(&now);
        currentTime.pop_back();
    }

    /**
     * Another thread for output to file
     */
    void LogHandler::outputEngine() {
        while(true) {
            if( ! isEngineReady) {
                // make sure engine is up
                std::lock_guard<std::mutex> lck(engineMtx);  // protect isEngineReady
                isEngineReady = true;
                engineCV.notify_one();
            }

            {
                // get write buffer
                std::unique_lock<std::mutex> logLck(logMtx);  // protect logReadBuffer
                while(logWriteBuffer.empty()) {
                    logCV.wait_for(logLck, flushFrequency);
                    logWriteBuffer.swap(logReadBuffer);
                    if(isCloseEngine && logWriteBuffer.empty()) exit(0);
                    // fresh time
                    freshCurrentTime();
                }
            }

            while( ! logWriteBuffer.empty()) {
                std::shared_ptr<LogEntity> logMsg = logWriteBuffer.front();
                std::string outputMsg = logMsg->logMsg;

                // no need to use mutex protect output configuration
                // and log stream descriptor
                // because it cannot be modified when Handler is running
                if(output.at(Output::CONSOLE)) {
                    outputToConsole(outputMsg);
                }

                if(output.at(Output::FILE)) {
                    outputToFile(outputMsg);
                }

                logWriteBuffer.pop();
            }

            if(output.at(Output::FILE)) {
                logStream << std::flush;
            }
        }
    }

    /**
     * Print log to console
     */
    void LogHandler::outputToConsole(const std::string& logMsg) const {
        std::cout << logMsg;
    }

    /**
     * Print log to log file
     */
    void LogHandler::outputToFile(const std::string& logMsg) {
        logStream << logMsg;
    }

    /**
     * get formatted output log
     */
    std::string LogHandler::formatOutput(std::shared_ptr<LogEntity> logMsg) const {
        char buffer[MaxMsgSize];
        snprintf(buffer, MaxMsgSize, "%s -> [%s::%s::%u] %s >> %s\n",
                 getLogLevel(logMsg->level).c_str(),
                 logMsg->filename.c_str(),
                 logMsg->funcname.c_str(),
                 logMsg->line,
                 logMsg->time.c_str(),
                 logMsg->message.c_str());
        return buffer;
    }

    LogHandler& LogHandler::getHandler() {
        static LogHandler instance;
        return instance;
    }
}
