// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <stdarg.h>

#include <string>

#include "og3/compiler_definitions.h"

namespace og3 {

// Logger is a very simple interface for handling text logs.
// Subclasses should override the pure-virtual log() function.
// define LOG_DEBUG() for debug() logs to be processed, or
//  LOG_DISABLE to disable all logging.
class Logger {
 public:
  virtual void log(const char* msg) = 0;

  void log(const std::string& str) {
#ifndef LOG_DISABLE
    this->log(str.c_str());
#endif
  }

  void logv(const char* format, va_list ap) {
#ifndef LOG_DISABLE
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, ap);
    this->log(buffer);
#endif
  }
  void logf(const char* format, ...) OG3_PRINTF_FORMAT(2) {
#ifndef LOG_DISABLE
    va_list ap;
    va_start(ap, format);
    logv(format, ap);
    va_end(ap);
#endif
  }

#ifdef LOG_DEBUG
  void debug(const char* text) { log(text); }
  void debugf(const char* format, ...) OG3_PRINTF_FORMAT(2) {
    va_list ap;
    va_start(ap, format);
    logv(format, ap);
    va_end(ap);
  }
#else
  inline void debug(const char* text) {}
  inline void debugf(const char* format, ...) OG3_PRINTF_FORMAT(2) {}
#endif
};

class NullLogger : public Logger {
 public:
  NullLogger() = default;
  void log(const char* msg) final {}
};

}  // namespace og3
