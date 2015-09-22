#ifndef LOGGINGLIB_H
#define LOGGINGLIB_H

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
     * Transfer Log Level to std::string
     */
    inline std::string getLogLevel(const Level& level) {
        switch(level) {
        case Level::Debug: return "DEBUG";
        case Level::Info: return "INFO";
        case Level::Warn: return "WARN";
        case Level::Error: return "ERROR";
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
