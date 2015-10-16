Logging plus plus implements in C++
[Blog article about this](http://override.rocks/2015/09/13/logging-in-cpp/)

#### Feature
- Multiple log level(Info, Debug, Warn, Error)
- High performance(about 155MB / 0.96s (multithreading))
- Thread-safe
- Flexible configuration

#### Install
```Shell
mkdir build
cd build
cmake ../
sudo make install
```

#### Output

- C++ stream style
```c++
Log(INFO) << "Hello" << " Gallon" << 1 << 2.1243;
```
`
$ INFO -> [test.cpp::main::71] Sun Sep 20 09:32:42 2015 >> Hello Gallon12.124300
`

#### Example
```c++
#include <logger/Logger.hpp>

using Logger::logger;
using Logger::Level::INFO;
using Logger::Level::DEBUG;
using Logger::Level::ERROR;
using Logger::Level::WARN;

int main(void) {
    logger.init();
    Log(INFO) << "Hello " << 1 << 2.1243;  // c++ stream style
}
```
