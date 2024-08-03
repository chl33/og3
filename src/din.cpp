// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/din.h"

#include <Arduino.h>

#include "og3/logger.h"

namespace og3 {

DIn::DIn(const char* name_, ModuleSystem* module_system_, uint8_t pin_, const char* description,
         VariableGroup* vg, bool publish, bool invert)
    : Module(name_, module_system_),
      m_pin(pin_),
      m_invert(invert),
      m_is_high(name_, false, description, vg, publish) {
  add_init_fn([this]() { pinMode(m_pin, INPUT); });
}

bool DIn::read() {
  const bool pin_is_high = (HIGH == digitalRead(m_pin));
  const bool is_high = m_invert ? !pin_is_high : pin_is_high;
  if (is_high != m_is_high.value()) {
    log()->debugf("%s -> %s", name(), is_high ? "high" : "low");
  }
  m_is_high = is_high;
  return is_high;
}

}  // namespace og3
