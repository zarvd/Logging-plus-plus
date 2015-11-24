#ifndef LOGGING_PLUS_PLUS_HELPER_H_
#define LOGGING_PLUS_PLUS_HELPER_H_

#include <string>
#include <deque>

namespace logger {

/**
 * Separate path into diretory and filename
 */
inline void PathToFile(const std::string& path, std::string& dir,
                       std::string& filename) {
  std::size_t found_pos = path.find_last_of("/");
  if (found_pos != std::string::npos) {
    dir = path.substr(0, found_pos);
    filename = path.substr(found_pos + 1);
  } else {
    dir = "";
    filename = path;
  }
}

/**
 * Combine diretory and filename
 */
inline std::string DirAndFileToPath(const std::string& dir,
                                    const std::string& filename) {
  if (dir == "") {
    return "./" + filename;
  } else if (dir.back() == '/') {
    return dir + filename;
  } else {
    return dir + "/" + filename;
  }
}
}

#endif /* LOGGING_PLUS_PLUS_HELPER_H_ */
