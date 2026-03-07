// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>
#include <stdarg.h>

#include "og3/compiler_definitions.h"

namespace og3 {

/**
 * @brief Base interface for application-wide logging.
 *
 * Subclasses must implement the pure virtual `log()` method.
 * Supports printf-style formatting and conditional debug levels.
 */
class Logger {
 public:
  virtual ~Logger() = default;

  /**
   * @brief Core logging function.
   * @param msg The C-string message to log.
   */
  virtual void log(const char* msg) = 0;

  /** @brief Logs an Arduino String. */
  void log(const String& str) {
#ifndef LOG_DISABLE
    log(str.c_str());
#endif
  }

  /** @brief Logs a message with variable arguments (va_list). */
  void logv(const char* format, va_list ap) {
#ifndef LOG_DISABLE
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, ap);
    log(buffer);
#endif
  }

  /** @brief Logs a printf-style formatted message. */
  void logf(const char* format, ...) OG3_PRINTF_FORMAT(2) {
#ifndef LOG_DISABLE
    va_list ap;
    va_start(ap, format);
    logv(format, ap);
    va_end(ap);
#endif
  }

#ifdef LOG_DEBUG
  /** @brief Logs a debug message (only if LOG_DEBUG is defined). */
  void debug(const char* text) { log(text); }
  /** @brief Logs a formatted debug message (only if LOG_DEBUG is defined). */
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

/**
 * @brief A logger implementation that ignores all messages.
 */
class NullLogger : public Logger {
 public:
  NullLogger() = default;
  void log(const char* msg) final {}
};

}  // namespace og3
