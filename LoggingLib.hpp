#ifndef LOGGINGLIB_H
#define LOGGINGLIB_H

#include <iostream>

#include <fstream>
#include <string>
#include <map>
#include <deque>

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
enum class Level {DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3};

/**
 * Transfer Log Level to std::string
 */
inline std::string getLogLevel(const Level& level) {
    switch(level) {
    case Level::DEBUG: return "DEBUG";
    case Level::INFO: return "INFO";
    case Level::WARN: return "WARN";
    case Level::ERROR: return "ERROR";
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

#endif /* LOGGINGLIB_H */
