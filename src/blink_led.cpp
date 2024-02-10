// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/blink_led.h"

#include <Arduino.h>

#include <functional>

namespace og3 {

// Class which can blink an LED.
BlinkLed::BlinkLed(const char* name_, uint8_t led, App* app, unsigned onMsec, bool onLow,
                   unsigned offMsec)
    : Module(name_, &app->module_system()),
      m_led(led),
      m_on_msec(onMsec),
      m_off_msec(offMsec ? offMsec : onMsec),
      m_on_low(onLow),
      m_scheduler([this]() { blinkCallback(); }, &app->tasks()) {
  add_init_fn([this]() {
    pinMode(m_led, OUTPUT);
    off();
  });
}

void BlinkLed::on() {
  m_is_on = true;
  digitalWrite(m_led, m_on_low ? LOW : HIGH);
}
void BlinkLed::off() {
  m_is_on = false;
  digitalWrite(m_led, m_on_low ? HIGH : LOW);
}
void BlinkLed::blink(uint8_t num) {
  m_num_blinks = num;
  on();
  m_scheduler.runIn(m_on_msec);
}
void BlinkLed::delayedBlink(unsigned msec, uint8_t num) {
  m_num_blinks = num;
  m_scheduler.runIn(msec);
}

void BlinkLed::blinkCallback() {
  if (m_is_on) {
    off();
    if (m_num_blinks > 0) {
      m_num_blinks -= 1;
      // Turn back on in m_off_msec.
      m_scheduler.runIn(m_off_msec);
    }
  } else {
    on();
    // Turn back off in m_on_msec.
    m_scheduler.runIn(m_on_msec);
  }
}

}  // namespace og3
