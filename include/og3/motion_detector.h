// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/din.h"
#include "og3/ha_discovery.h"

namespace og3 {

// A wrapper for a digital input reading the value from a motion detector sensor
class MotionDetector : public DIn {
 public:
  MotionDetector(const char* name, ModuleSystem* module_system, uint8_t pin,
                 const char* description, VariableGroup& vg, bool publish = true,
                 bool ha_discovery = true);

 private:
  HADiscovery* m_ha_discovery = nullptr;
};

}  // namespace og3
