// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

// A wrapper for reading a digital input pin.
class DIn : public Module {
 public:
  DIn(const char* name_, ModuleSystem* module_system_, uint8_t pin_, const char* description,
      VariableGroup* vg, bool publish = true, bool invert = false);

  // Read the dio pin and return isHigh().
  bool read();

  uint8_t pin() const { return m_pin; }
  const Variable<bool>& isHighVar() const { return m_is_high; }
  bool isHigh() const { return m_is_high.value(); }

 private:
  const uint8_t m_pin;
  const bool m_invert;
  BinarySensorVariable m_is_high;
};

}  // namespace og3
