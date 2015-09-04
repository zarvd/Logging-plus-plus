#ifndef LIB_H
#define LIB_H


#include <string>

namespace Lib {
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

#endif /* LIB_H */
