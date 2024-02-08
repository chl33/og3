// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/app.h"
#include "og3/module.h"

namespace og3 {

// A class for control of an LED hooked to an output pin.
class BlinkLed : public Module {
 public:
  BlinkLed(const char* name_, uint8_t led, App* app, unsigned onMsec, bool onLow = true,
           unsigned offMsec = 0);
  void on();
  void off();
  void blink(uint8_t num = 1);
  void delayedBlink(unsigned msec, uint8_t num = 1);

 private:
  void scheduleCallback(unsigned msec);
  void blinkCallback();

  const uint8_t m_led;
  const unsigned m_on_msec;
  const unsigned m_off_msec;
  const bool m_on_low;
  TaskScheduler m_scheduler;
  bool m_is_on = false;
  uint8_t m_num_blinks = 0;
};

}  // namespace og3
