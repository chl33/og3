// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

/**
 * @brief A module for reading and monitoring a digital input pin.
 *
 * DIn encapsulates a digital input, providing debouncing (implicitly through the update loop)
 * and integration with the Variable system for monitoring and MQTT publishing.
 */
class DIn : public Module {
 public:
  /**
   * @brief Constructs a DIn module.
   * @param name_ Unique name for the module and its variable.
   * @param module_system_ The ModuleSystem to register with.
   * @param pin_ The hardware GPIO pin number.
   * @param description Human-readable description of the input.
   * @param vg The VariableGroup to add the state variable to.
   * @param publish true to publish the state via MQTT.
   * @param invert true if the input logic is inverted (active-low).
   */
  DIn(const char* name_, ModuleSystem* module_system_, uint8_t pin_, const char* description,
      VariableGroup& vg, bool publish = true, bool invert = false);

  /**
   * @brief Reads the current state of the GPIO pin.
   * @return The logic level (respecting the invert flag).
   */
  bool read();

  /** @return The GPIO pin number. */
  uint8_t pin() const { return m_pin; }
  /** @return Constant reference to the boolean variable tracking the pin state. */
  const Variable<bool>& isHighVar() const { return m_is_high; }
  /** @return Current state of the input (true = active/high, false = inactive/low). */
  bool isHigh() const { return m_is_high.value(); }

 private:
  const uint8_t m_pin;
  const bool m_invert;
  BinarySensorVariable m_is_high;
};

}  // namespace og3
