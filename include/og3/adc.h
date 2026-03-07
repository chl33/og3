// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

/**
 * @brief Wrapper for an analog-to-digital input.
 */
class Adc : public Module {
 public:
#ifdef ARDUINO_ARCH_ESP32
  /**
   * @brief ADC resolution for ESP32.
   */
  static constexpr unsigned kResolution = 0x1000;
#elif defined(ARDUINO_ARCH_ESP8266)
  /**
   * @brief ADC resolution for ESP8266.
   */
  static constexpr unsigned kResolution = 0x0400;
#else
  /**
   * @brief Default ADC resolution.
   */
  static constexpr unsigned kResolution = 0x0400;
#endif
  /**
   * @brief Maximum count value for the ADC.
   */
  static constexpr unsigned kMaxCounts = kResolution - 1;

  /**
   * @brief Construct a new Adc object.
   * @param name_ The name of the module.
   * @param pin_ The ADC pin to read from.
   * @param module_system_ The module system instance.
   * @param description Description of the ADC variable.
   * @param var_flags Flags for the ADC variable.
   * @param vg The variable group to add the ADC variable to.
   */
  Adc(const char* name_, uint8_t pin_, ModuleSystem* module_system_, const char* description,
      unsigned var_flags, VariableGroup& vg);

  Adc(const Adc&) = delete;

  /**
   * @brief Read the current value from the ADC.
   * @return The current ADC count value.
   */
  unsigned read();

  /**
   * @brief Get the ADC counts variable.
   * @return The constant ADC counts variable.
   */
  const Variable<unsigned>& countsVar() const { return m_counts; }

  /**
   * @brief Get the ADC counts variable.
   * @return The ADC counts variable.
   */
  Variable<unsigned>& countsVar() { return m_counts; }

  /**
   * @brief Get the current ADC count value.
   * @return The current ADC count value.
   */
  unsigned counts() const { return m_counts.value(); }

 private:
  const uint8_t m_pin;
  Variable<unsigned> m_counts;
};

}  // namespace og3
