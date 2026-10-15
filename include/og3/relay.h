// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/dout.h"
#include "og3/tasks.h"

namespace og3 {

/**
 * @brief Module for controlling a relay via a digital output pin.
 *
 * Relay extends DOut to provide relay-specific functionality, such as
 * timed "on" cycles and tracking the last time the relay was activated.
 */
class Relay : public DOut {
 public:
  /** @brief Defines the logic level that activates the relay. */
  enum class OnLevel {
    kHigh = 0,  ///< Relay is on when output pin is high.
    kLow = 1,   ///< Relay is on when output pin is low.
  };

  /**
   * @brief Constructs a Relay module.
   * @param name_ Unique name for the relay.
   * @param tasks Pointer to the Tasks module for scheduling timed-off.
   * @param pin_ Hardware GPIO pin.
   * @param description Human-readable description.
   * @param publish true to publish state via MQTT.
   * @param vg VariableGroup to add the state variable to.
   * @param on_level Logic level for "on" state.
   */
  Relay(const std::string& name, Tasks* tasks, uint8_t pin_, const char* description, bool publish,
        VariableGroup& vg, OnLevel on_level = OnLevel::kHigh);

  /**
   * @brief Activates the relay.
   * @param on_msec If > 0, the relay will automatically turn off after this many milliseconds.
   * @param off_fn Optional callback to execute when the relay turns off.
   * @return true if the relay was previously off.
   */
  bool turnOn(int on_msec = 0, const std::function<void()>& off_fn = nullptr);

  /**
   * @brief Schedules the relay to turn off.
   * @param msec Time from now to turn off.
   * @param fn Optional callback.
   */
  void turnOffIn(int msec, const std::function<void()>& fn = nullptr);

  /** @brief Immediately turns the relay off. */
  void turnOff();

  /** @return true if the relay is currently energized. */
  bool isOn() const { return isHigh() == (m_on_level == OnLevel::kHigh); }

  /** @return System time (millis) when the relay was last turned on (or current time if on). */
  unsigned long lastOnMsec() const { return isOn() ? millis() : m_last_on_msec; }

 private:
  const OnLevel m_on_level;
  unsigned long m_last_on_msec = 0;
  std::function<void()> m_off_callback;
  Tasks* m_tasks;
};

}  // namespace og3
