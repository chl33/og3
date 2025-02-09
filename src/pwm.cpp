// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#ifndef NATIVE
#include "og3/pwm.h"

#include <Arduino.h>

namespace og3 {
namespace {
uint16_t _max_counts(uint8_t resolution) { return (static_cast<uint32_t>(1) << resolution) - 1; }
}  // namespace

Pwm::Pwm(const char* name, uint8_t pwm_pin,
#ifdef ARDUINO_ARCH_ESP32
         uint8_t pwm_channel, uint8_t pwm_resolution,
#endif
         ModuleSystem* module_system, double pwm_frequency)
    : Module(name, module_system),
      m_pwm_pin(pwm_pin),
#ifdef ARDUINO_ARCH_ESP32
      m_pwm_channel(pwm_channel),
      m_pwm_resolution(pwm_resolution),
      m_max_counts(_max_counts(pwm_resolution)),
#else
      m_max_counts(_max_counts(8)),
#endif
      m_pwm_frequency(pwm_frequency) {
  add_init_fn([this]() {
#ifdef ARDUINO_ARCH_ESP32
    // Sets up a channel (0-15), a PWM duty cycle frequency, and a PWM resolution (1 - 16 bits)
    ledcSetup(m_pwm_channel, m_pwm_frequency, m_pwm_resolution);
    ledcAttachPin(m_pwm_pin, m_pwm_channel);
#else
    pinMode(m_pwm_pin, OUTPUT);
#endif
  });

  add_start_fn([this]() { setDuty(m_pwm_frequency); });
}

void Pwm::setDuty(unsigned counts) {
  m_duty_counts = counts;
#ifdef ARDUINO_ARCH_ESP32
  ledcWrite(m_pwm_channel, counts);
#else
  analogWrite(m_pwm_pin, counts);
#endif
}

void Pwm::setDutyF(float fraction) { setDuty(static_cast<unsigned>(fraction * m_max_counts)); }

}  // namespace og3

#endif  // !NATIVE
