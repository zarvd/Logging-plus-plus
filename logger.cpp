#include "logger.hpp"

namespace Logger {
    std::mutex logMtx;
    LogHandler LoggingHandler;

    LogHandler::LogHandler() :
        logLevel(Level::Info),
        isWriteToFile(true),
        isPrintToConsole(true),
        logDir(""),
        logFile("app.log"),
        logMsg(Log()) {}

    LogHandler::~LogHandler() {
        if(logStream.is_open()) {
            logStream.close();
        }
    }


    void LogHandler::init() {
        if(logStream.is_open()) {
            logStream.close();
        }
        if(isWriteToFile) {
            openLogStream();
        }
    }

    void LogHandler::setWriteToFile(const bool& isWrite) {
        if(isWriteToFile && logStream.is_open()) {
            logStream.close();
        }
        isWriteToFile = isWrite;
        const std::string logPath = dirAndFileToPath(logDir, logFile);
        logStream = std::ofstream(logPath, std::ofstream::out | std::ofstream::app);
    }

    void LogHandler::setLogFile(const std::string& logPath) {
        if(logStream.is_open()) {
            logStream.close();
        }

        isWriteToFile = true;

        pathToFile(logPath, logDir, logFile);
    }

    void LogHandler::setLogLevel(const Level& level) {
        logLevel = level;
    }

    void LogHandler::log(const Level& level, const std::string& msg) {
        if(level < logLevel) return;
        if(isWriteToFile && ! logStream.is_open()) {
            throw std::domain_error("log stream is not open");
        }
        logMtx.lock();
        auto nowTime = std::chrono::system_clock::now();
        logMsg.time = std::chrono::system_clock::to_time_t(nowTime);
        logMsg.level = level;
        logMsg.message = msg;
        if(isWriteToFile) {
            writeToFile();
        }
        if(isPrintToConsole) {
            printToConsole();
        }
        logMtx.unlock();
    }

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

    void LogHandler::printToConsole() const {
        std::cout << getLogLevel(logMsg.level) << " -> "
                  << getTime(logMsg.time) << " >> "
                  << logMsg.message
                  << std::endl;
    }

    void LogHandler::writeToFile() {
        if( ! logStream.is_open()) {
            throw std::domain_error("log stream is not open");
        }
        logStream << getLogLevel(logMsg.level) << " -> "
                  << getTime(logMsg.time) << " >> "
                  << logMsg.message
                  << std::endl;
    }
}
