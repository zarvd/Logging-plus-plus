Logging plus plus implements in C++

#### Feature
- Multiple log level(Info, Debug, Warn, Error)
- High performance(about 1000k / 1 second)
- Thread-safe
- Flexible configuration

#### Usage
```c++
#include "logger.hpp"

using Logger::LoggingHandler;
using Logger::logInfo;
using Logger::logDebug;
using Logger::logWarn;
using Logger::logError;
using logLevel = Logger::Level;

int main(void) {
    LoggingHandler.init();
    log(logLevel::Info)("Log test");
    logInfo("test");
}
```
