#include "logger.hpp"


namespace Logger {
    LogHandler::LogHandler() :
        isStop(true),
        MaxMsgSize(200),
        maxBufferSize(50),
        flushFrequency(3),
        logDir(""),
        logFile("app.log"),
        logCount(0),
        logLevel(Level::Info),
        logReadBuffer(),
        logWriteBuffer(),
        output({{Output::FILE, true},
                {Output::CONSOLE, true}}) {}

    LogHandler::~LogHandler() {
        if( ! isStop) stop();
    }

    /**
     * Before using a logger, you need to initialize it.
     * it will open a file Stream if it is allowed to write to a log file
     */
    void LogHandler::init() {
        std::lock_guard<std::mutex> lck(logMtx);
        if( ! isStop) {
            throw std::logic_error("Logging handler had inited");
        }
        isStop = false;
        outputThread = std::thread(&LogHandler::outputEngine, this);
        if(logStream.is_open()) {
            logStream.close();
        }
        if(output.at(Output::FILE)) {
            openLogStream();
        }
    }

    void LogHandler::stop() {
        if(isStop) {
            throw std::logic_error("Logging handler had stopped");
        }
        isStop = true;
        outputThread.join();
        if(logStream.is_open()) {
            logStream.close();
        }
    }

    /**
     * Setting output allowance
     */
    void LogHandler::setOutput(const Output& output, const bool& isAllowed) {
        std::lock_guard<std::mutex> lck(logMtx);
        switch(output) {
        case Output::FILE:
            if( ! isAllowed && logStream.is_open()) {
                logStream.close();
            } else if( ! this->output.at(Output::FILE) && isAllowed) {
                const std::string logPath = dirAndFileToPath(logDir, logFile);
                logStream = std::ofstream(logPath, std::ofstream::out | std::ofstream::app);
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
        logLevel = level;
    }

    /**
     * Log operation
     */
    void LogHandler::log(const Level& level, const std::string& msg) {
        // create log message before lock mutex which may block
        std::shared_ptr<Log> logMsg(new Log);
        logMsg->index = ++ logCount;
        logMsg->time = currentTime;
        logMsg->level = level;
        logMsg->message = msg;

        // it may block
        std::unique_lock<std::mutex> lck(logMtx);

        if(isStop) {
            throw std::logic_error("logging handler haven't inited");
        }
        if(level < logLevel) return;
        if(output.at(Output::FILE) && ! logStream.is_open()) {
            throw std::domain_error("LogHandler::log(): log stream is not open");
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

    /**
     * Another thread for output to file
     */
    void LogHandler::outputEngine() {
        while( ! isStop) {
            std::unique_lock<std::mutex> logLck(logMtx);
            while(logWriteBuffer.empty()) {
                logCV.wait_for(logLck, flushFrequency);
                logWriteBuffer.swap(logReadBuffer);
                if(isStop && logWriteBuffer.empty()) exit(0);
            }

            // fresh time
            currentTime = getCurrentTime();

            while( ! logWriteBuffer.empty()) {
                std::shared_ptr<Log> logMsg = logWriteBuffer.front();
                std::string outputMsg = formatOutput(logMsg);
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
        if( ! logStream.is_open()) {
            throw std::domain_error("LogHandler::outputToFile: log stream is not open");
        }
        logStream << logMsg;
    }

    /**
     * get formatted output log
     */
    std::string LogHandler::formatOutput(std::shared_ptr<Log> logMsg) const {
        char buffer[MaxMsgSize];
        snprintf(buffer, MaxMsgSize, "[%lu] %s -> %s >> %s\n",
                 logMsg->index,
                 getLogLevel(logMsg->level).c_str(),
                 logMsg->time.c_str(),
                 logMsg->message.c_str());
        return buffer;
    }

    LogHandler LoggingHandler;  // Global logging handler
}
