// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#ifndef NATIVE

#include <functional>

#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/variable.h"

namespace og3 {

/**
 * @brief Module for generating Pulse Width Modulation (PWM) signals.
 *
 * Pwm provides an abstraction for hardware PWM control (e.g. `ledc` on ESP32
 * or `analogWrite` on ESP8266). It supports setting duty cycle by raw counts
 * or percentage.
 */
class Pwm : public Module {
 public:
  /**
   * @brief Constructs a Pwm module.
   * @param name Unique name for the module.
   * @param pwm_pin The hardware GPIO pin.
   * @param pwm_channel (ESP32 only) The LEDC channel (0-15).
   * @param pwm_resolution (ESP32 only) The timer resolution in bits (1-16).
   * @param module_system The ModuleSystem to register with.
   * @param pwm_frequency The PWM frequency in Hz.
   */
  Pwm(const char* name, uint8_t pwm_pin,
#ifdef ARDUINO_ARCH_ESP32
      uint8_t pwm_channel,     // 0-15
      uint8_t pwm_resolution,  // 1-16
#endif
      ModuleSystem* module_system, double pwm_frequency = 500.0);

  /** @brief Sets the raw duty cycle counts. */
  void setDuty(unsigned counts);
  /** @brief Sets the duty cycle as a percentage (0.0 to 100.0). */
  void setDutyF(float percent);

 private:
  const char* m_name;
  const uint8_t m_pwm_pin;
#ifdef ARDUINO_ARCH_ESP32
  const uint8_t m_pwm_channel;
  const uint8_t m_pwm_resolution;
#endif
  const uint16_t m_max_counts;
  const double m_pwm_frequency;
  unsigned m_duty_counts = 0;
};

}  // namespace og3

#endif  // !NATIVE
