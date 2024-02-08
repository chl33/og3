// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

// Wrapper for an analog-to-digital input
class Adc : public Module {
 public:
#ifdef ARDUINO_ARCH_ESP32
  static constexpr unsigned kResolution = 0x1000;
#elif defined(ARDUINO_ARCH_ESP8266)
  static constexpr unsigned kResolution = 0x0400;
#else
  static constexpr unsigned kResolution = 0x0400;
#endif
  static constexpr unsigned kMaxCounts = kResolution - 1;

  Adc(const char* name_, uint8_t pin_, ModuleSystem* module_system_, const char* description,
      unsigned var_flags, VariableGroup* vg);

  Adc(const Adc&) = delete;

  unsigned read();

  const Variable<unsigned>& countsVar() const { return m_counts; }
  Variable<unsigned>& countsVar() { return m_counts; }
  unsigned counts() const { return m_counts.value(); }

 private:
  const uint8_t m_pin;
  Variable<unsigned> m_counts;
};

}  // namespace og3
