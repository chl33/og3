// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <Arduino.h>

#include "og3/logger.h"

namespace og3 {

// A logger which sends output to the uart.
class SerialLogger : public Logger {
 public:
  SerialLogger() {}
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
