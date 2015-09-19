Logging plus plus implements in C++
[Blog article about this](http://override.rocks/2015/09/13/logging-in-cpp/)

#### Feature
- Multiple log level(Info, Debug, Warn, Error)
- High performance(about 2000k items / 0.95s (multithreading))
- Thread-safe
- Flexible configuration

#### Output
- C/Java function style
```c++
logInfo("Log test: %d", i);
```
- C++ stream style
```c++
log(logLevel::Info) << "Hello " << 1 << 2.1243 << Input::FIN;
```

#### Example
```c++
#include "logger.hpp"

using Logger::LoggingHandler;
using Logger::logInfo;
using Logger::logDebug;
using Logger::logWarn;
using Logger::logError;
using Logger::Input;
using logLevel = Logger::Level;

int main(void) {
    LoggingHandler.init();
    for(unsigned i = 1; i < 100000; ++ i) {
        log(logLevel::Info) << "Hello " << 1 << 2.1243 << Input::FIN;  // c++ stream style
        logInfo("Log test: %d", i);  // format output
    }
}
```
