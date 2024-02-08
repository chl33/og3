// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#ifndef NATIVE

#include <functional>

#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/variable.h"

namespace og3 {

// Pwm is a wrapper for generating PWM signals from a Dout pin.
class Pwm : public Module {
 public:
  Pwm(const char* name, uint8_t pwm_pin,
#ifdef ARDUINO_ARCH_ESP32
      uint8_t pwm_channel,     // 0-15
      uint8_t pwm_resolution,  // 1-16
#endif
      ModuleSystem* module_system, double pwm_frequency = 500.0);

  void setDuty(unsigned counts);
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
