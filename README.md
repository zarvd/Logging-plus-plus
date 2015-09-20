Logging plus plus implements in C++
[Blog article about this](http://override.rocks/2015/09/13/logging-in-cpp/)

#### Feature
- Multiple log level(Info, Debug, Warn, Error)
- High performance(about 146MB / 0.98s (multithreading))
- Thread-safe
- Flexible configuration

#### Output
- C++ stream style
```c++
Log(logLevel::Info) << "Hello" << " Gallon" << 1 << 2.1243 << Input::FIN;
```
`
$ [1] INFO -> [test.cpp::main::71] Sun Sep 20 09:32:42 2015 >> Hello Gallon12.124300
`

#### Example
```c++
#include "logger.hpp"

using Logger::LoggingHandler;
using Logger::Input;
using logLevel = Logger::Level;

int main(void) {
    LoggingHandler.init();
    for(unsigned i = 1; i < 100000; ++ i) {
        Log(logLevel::Info) << "Hello " << 1 << 2.1243 << Input::FIN;  // c++ stream style
    }
}
```
