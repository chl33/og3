// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/din.h"
#include "og3/ha_dependencies.h"
#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/util.h"
#include "og3/variable.h"

namespace og3 {

// Pir is a wrapper for PIR sensors.
class Pir : public Module {
 public:
  Pir(const char* module_name, const char* motion_name, ModuleSystem* module_system, uint8_t pin,
      const char* description, VariableGroup& vg, bool publish, bool ha_discovery);

  void read();

  // Whether the last value of read() detected the PIR motion pin was high.
  bool motion() const { return m_din.isHigh(); }

  // Setup an interrupt to be raised when the DIO pin goes high, and
  //  register a thunk to be called-back on the interrupt.
  // The callback happens via the Tasks system, so not inside the interrupt itself.
  void callOnMotion(const Thunk& fn);

 private:
  bool haDeclare(HADiscovery* had, JsonDocument* json);

#ifndef NATIVE
  static void IRAM_ATTR _onMotion();
#endif

  static bool s_interrupt_setup;

  HADependencies m_dependencies;
  const std::string m_din_name;
  DIn m_din;
};

}  // namespace og3
