// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

/**
 * @brief A module for controlling a digital output pin.
 *
 * DOut provides a simple interface for setting the state of a GPIO pin and
 * tracking that state through the Variable system.
 */
class DOut : public Module {
 public:
  /**
   * @brief Constructs a DOut module.
   * @param name Unique name for the module and its variable.
   * @param initial_val The starting state of the pin.
   * @param module_system_ The ModuleSystem to register with.
   * @param pin_ The hardware GPIO pin number.
   * @param description Human-readable description of the output.
   * @param publish true to publish the state via MQTT.
   * @param vg The VariableGroup to add the state variable to.
   */
  DOut(const std::string& name, bool initial_val, ModuleSystem* module_system_, uint8_t pin_,
       const char* description, bool publish, VariableGroup& vg);

  /**
   * @brief Sets the output state of the GPIO pin.
   * @param is_high true for HIGH, false for LOW.
   */
  void set(bool is_high);

  /** @return Constant reference to the boolean variable tracking the pin state. */
  const Variable<bool>& isHighVar() const { return m_is_high; }
  /** @return true if the pin is currently HIGH. */
  bool isHigh() const { return m_is_high.value(); }

 private:
  const uint8_t m_pin;
  BinarySensorVariable m_is_high;
};

}  // namespace og3
