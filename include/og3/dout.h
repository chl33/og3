// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

// A wrapper for control of a digital output pin.
class DOut : public Module {
 public:
  DOut(const char* name_, bool initial_val, ModuleSystem* module_system_, uint8_t pin_,
       const char* description, bool publish, VariableGroup& vg);

  void set(bool is_high);

  const Variable<bool>& isHighVar() const { return m_is_high; }
  bool isHigh() const { return m_is_high.value(); }

 private:
  const uint8_t m_pin;
  BinarySensorVariable m_is_high;
};

}  // namespace og3
