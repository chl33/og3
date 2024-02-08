// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/adc.h"

#include <Arduino.h>

#include "og3/logger.h"

namespace og3 {

Adc::Adc(const char* name_, uint8_t pin_, ModuleSystem* module_system_, const char* description,
         unsigned var_flags, VariableGroup* vg)
    : Module(name_, module_system_),
      m_pin(pin_),
      m_counts(name_, false, "counts", description, var_flags, vg) {
  add_init_fn([this]() { pinMode(m_pin, INPUT); });
}

unsigned Adc::read() {
  m_counts = analogRead(m_pin);
  return m_counts.value();
}

}  // namespace og3
