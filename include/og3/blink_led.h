// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/app.h"
#include "og3/module.h"

namespace og3 {

/**
 * @brief A class for control of an LED hooked to an output pin.
 */
class BlinkLed : public Module {
 public:
  /**
   * @brief Construct a new BlinkLed object.
   * @param name_ The name of the module.
   * @param led The pin the LED is connected to.
   * @param app The application instance.
   * @param onMsec Duration in milliseconds for the LED to be on during a blink.
   * @param onLow Whether the LED is active low.
   * @param offMsec Duration in milliseconds for the LED to be off during a blink.
   */
  BlinkLed(const std::string& name_, uint8_t led, App* app, unsigned onMsec, bool onLow = true,
           unsigned offMsec = 0);

  /**
   * @brief Turn the LED on.
   */
  void on();

  /**
   * @brief Turn the LED off.
   */
  void off();

  /**
   * @brief Blink the LED a specified number of times.
   * @param num The number of times to blink.
   */
  void blink(uint8_t num = 1);

  /**
   * @brief Schedule a blink after a delay.
   * @param msec Delay in milliseconds.
   * @param num The number of times to blink.
   */
  void delayedBlink(unsigned msec, uint8_t num = 1);

 private:
  /**
   * @brief Callback for blink scheduling.
   */
  void blinkCallback();

  const uint8_t m_led;
  const unsigned m_on_msec;
  const unsigned m_off_msec;
  const bool m_on_low;
  bool m_is_on = false;
  uint8_t m_num_blinks = 0;
  TaskScheduler m_scheduler;
};

}  // namespace og3
