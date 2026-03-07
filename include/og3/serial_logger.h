// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <Arduino.h>

#include "og3/logger.h"

namespace og3 {

/**
 * @brief A Logger implementation that outputs to the hardware Serial (UART) port.
 *
 * Automatically initializes Serial at 115200 baud on the first log call if not
 * already started.
 */
class SerialLogger : public Logger {
 public:
  /** @brief Constructs a SerialLogger. */
  SerialLogger() {}

  /**
   * @brief Logs a message to Serial.
   * @param msg The C-string to log.
   */
  void log(const char* msg) final {
    if (!m_initialized) {
      Serial.begin(115200);
      m_initialized = true;
    }
    Serial.println(msg);
  }

 private:
  bool m_initialized = false;
};

}  // namespace og3
