#include "logger.hpp"


namespace Logger {
    LogHandler::LogHandler() :
        MaxBufferSize(5),
        logCount(0),
        logDir(""),
        logFile("app.log"),
        logLevel(Level::Info),
        logReadBuffer(),
        logWriteBuffer(),
        output({{Output::FILE, true},
                {Output::CONSOLE, true}}) {}

    LogHandler::~LogHandler() {
        stop();
    }

    /**
     * Before using a logger, you need to initialize it.
     * it will open a file Stream if it is allowed to write to a log file
     */
    void LogHandler::init() {
        std::lock_guard<std::mutex> lck(logMtx);
        outputThread = std::thread(&LogHandler::outputEngine, this);
        if(logStream.is_open()) {
            logStream.close();
        }
        if(output.at(Output::FILE)) {
            openLogStream();
        }
    }

    void LogHandler::stop() {
        if(logStream.is_open()) {
            logStream.close();
        }
        outputThread.join();
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
        std::unique_lock<std::mutex> lck(logMtx);

        if(level < logLevel) return;
        if(output.at(Output::FILE) && ! logStream.is_open()) {
            throw std::domain_error("LogHandler::log(): log stream is not open");
        }
        auto nowTime = std::chrono::system_clock::now();
        std::shared_ptr<Log> logMsg(new Log);
        logMsg->index = ++ logCount;
        logMsg->time = std::chrono::system_clock::to_time_t(nowTime);
        logMsg->level = level;
        logMsg->message = msg;
        logReadBuffer.push(logMsg);
        if(logReadBuffer.size() >= MaxBufferSize) {
            std::swap(logReadBuffer, logWriteBuffer);
            cv.notify_one();
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
        if(access(logDir.c_str(), F_OK) != 0 || access(logDir.c_str(), W_OK) == 0) {
            std::string dir("");
            for(unsigned short idx = 0; idx < this->logDir.length(); ++ idx) {
                // create new directory recusively
                const char& curChar = this->logDir[idx];
                if(curChar == '/' || idx + 1 == this->logDir.length()) {
                    dir = this->logDir.substr(0, idx);  // get new directory path
                    struct stat fileStat;
                    if(stat(dir.c_str(), &fileStat) < 0) {
                        throw "File doesn't exist";
                    }
                    if( ! S_ISDIR(fileStat.st_mode)) {
                        throw "Directory error";
                    }
                    if(mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                        throw "Cannot create directory";
                    }
                }
            }
        }
        logStream.open(dirAndFileToPath(logDir, logFile), std::ofstream::out | std::ofstream::app);
    }

    void LogHandler::outputEngine() {
        // std::lock_guard<std::mutex> lck (logMtx);
        while(true) {
            std::unique_lock<std::mutex> lck(logMtx);
            while(logWriteBuffer.empty()) {
                cv.wait(lck);
            }
            while( ! logWriteBuffer.empty()) {
                std::shared_ptr<Log> logMsg = logWriteBuffer.front();
                if(output.at(Output::FILE)) {
                    outputToFile(logMsg);
                }
                if(output.at(Output::CONSOLE)) {
                    outputToConsole(logMsg);
                }
                logWriteBuffer.pop();
            }
        }
    }

    /**
     * Print log to console
     */
    void LogHandler::outputToConsole(std::shared_ptr<Log> logMsg) const {
        std::cout << formatOutput(logMsg) << std::flush;
    }

    /**
     * Print log to log file
     */
    void LogHandler::outputToFile(std::shared_ptr<Log> logMsg) {
        if( ! logStream.is_open()) {
            throw std::domain_error("LogHandler::outputToFile: log stream is not open");
        }
        logStream << formatOutput(logMsg) << std::flush;
    }

    /**
     * get formatted output log
     */
    std::string LogHandler::formatOutput(std::shared_ptr<Log> logMsg) const {
        std::string buffer = "[" + std::to_string(logMsg->index) + "] "
            + getLogLevel(logMsg->level) + " -> "
            + getTime(logMsg->time) + " >> "
            + logMsg->message
            + '\n';
        return buffer;
    }

    LogHandler LoggingHandler;  // Global logging handler
}
