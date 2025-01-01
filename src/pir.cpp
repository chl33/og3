// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/pir.h"

#include <Arduino.h>

#include <functional>

#include "og3/ha_discovery.h"

namespace og3 {

namespace {
Thunk s_motion_callback;
}  // namespace

bool Pir::s_interrupt_setup = false;

Pir::Pir(const char* module_name, const char* motion_name, ModuleSystem* module_system, uint8_t pin,
         const char* description, VariableGroup& vg, bool publish, bool ha_discovery)
    : Module(module_name, module_system),
      m_din(motion_name, module_system, pin, description, vg, publish) {
  setDependencies(&m_dependencies);
  if (ha_discovery) {
    add_init_fn([this]() {
      if (m_dependencies.ok()) {
        m_dependencies.ha_discovery()->addDiscoveryCallback(
            [this](HADiscovery* had, JsonDocument* json) {
              return had->addMotionSensor(json, m_din.isHighVar());
            });
      }
    });
  }
}

// Interrupt routine causes onMotion to run in the main process
#ifndef NATIVE
void IRAM_ATTR Pir::_onMotion() { Tasks::run_next(s_motion_callback); }
#endif

// static
void Pir::callOnMotion(const Thunk& thunk) {
  if (!thunk) {
    return;
  }
  s_motion_callback = thunk;
  if (!s_interrupt_setup) {
#ifndef NATIVE
    attachInterrupt(digitalPinToInterrupt(m_din.pin()), Pir::_onMotion, CHANGE);
#endif
    s_interrupt_setup = true;
  }
}

void Pir::read() { m_din.read(); }

}  // namespace og3
