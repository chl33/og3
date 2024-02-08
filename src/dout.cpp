// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/dout.h"

#include <Arduino.h>

#include "og3/logger.h"

namespace og3 {
const char* strHigh(bool high) { return high ? "high" : "low"; }

DOut::DOut(const char* name_, bool initial_val, ModuleSystem* module_system_, uint8_t pin_,
           const char* description, bool publish, VariableGroup* vg)
    : Module(name_, module_system_),
      m_pin(pin_),
      m_is_high(name_, false, description, vg, publish) {
  add_init_fn([this]() { pinMode(m_pin, OUTPUT); });
  add_start_fn([this, initial_val]() { set(initial_val); });
}

void DOut::set(bool is_high) {
  digitalWrite(m_pin, is_high ? HIGH : LOW);
  if (m_is_high.value() != is_high) {
    log()->debugf("%s: %s -> %s", name(), strHigh(m_is_high.value()), strHigh(is_high));
    m_is_high = is_high;
  }
}

}  // namespace og3
