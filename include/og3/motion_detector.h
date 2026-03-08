// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/din.h"
#include "og3/ha_discovery.h"

namespace og3 {

/**
 * @brief Specialized digital input for PIR motion sensors.
 *
 * Extends DIn to add Home Assistant discovery support specifically
 * for motion detection binary sensors.
 */
class MotionDetector : public DIn {
 public:
  /**
   * @brief Constructs a MotionDetector.
   * @param name Unique name for the sensor.
   * @param module_system The ModuleSystem to register with.
   * @param pin The hardware pin the sensor is connected to.
   * @param description Human-readable description.
   * @param vg The VariableGroup to add the state variable to.
   * @param publish true to publish via MQTT.
   * @param ha_discovery true to enable Home Assistant discovery.
   */
  MotionDetector(const char* name, ModuleSystem* module_system, uint8_t pin,
                 const char* description, VariableGroup& vg, bool publish = true,
                 bool ha_discovery = true);

 private:
  HADiscovery* m_ha_discovery = nullptr;
};

}  // namespace og3
