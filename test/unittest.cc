#include "../include/logger.h"

using logger::LogLevel::TRACE;
using logger::LogLevel::INFO;
using logger::LogLevel::DEBUG;
using logger::LogLevel::ERROR;
using logger::LogLevel::WARN;
auto& logging = logger::LogHandler::GetHandler();

int main(void) {
  logging.Init();
  Log(INFO) << "Hello, world!";
  return 0;
}
