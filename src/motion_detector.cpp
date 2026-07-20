// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/motion_detector.h"

namespace og3 {

MotionDetector::MotionDetector(const std::string& name, ModuleSystem* module_system, uint8_t pin,
                               const char* description, VariableGroup& vg, bool publish,
                               bool ha_discovery)
    : DIn(name, module_system, pin, description, vg, publish) {
  if (ha_discovery) {
    require(HADiscovery::kName, &m_ha_discovery);
  }
  add_init_fn([this]() {
    if (m_ha_discovery) {
      m_ha_discovery->addDiscoveryCallback([this](HADiscovery* had, JsonDocument* json) {
        return had->addMotionSensor(json, isHighVar());
      });
    }
  });
}

}  // namespace og3
