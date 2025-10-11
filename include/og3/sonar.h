// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <math.h>

#include "og3/ha_discovery.h"
#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

inline float speedOfSound(float degC) { return 331.1 * sqrt(1.0 + degC / 273.15); }

constexpr float kSecPerUsec = 1e-6;

// A class for reaing a sonar sensor and and converting the measured ping time to a distance.
// This was used with a HC-SR04 sensor.
class Sonar : public Module {
 public:
  Sonar(const char* name, int trigPin, int echoPin, ModuleSystem* module_system, VariableGroup& vg,
        bool ha_discovery = true);

  void setTemp(float tempC);

  bool ok() const { return m_last_ping_ok; }
  // Distance measured in meters.
  float distance() const { return m_distance_m.value(); }
  float ping_usec() const { return m_delay_usec.value(); }
  void read();

  void ping();

 private:
  bool haDeclare(HADiscovery* had, JsonObject json);

  const uint8_t m_trigPin;
  const uint8_t m_echoPin;
  const String m_delay_name;
  // Ignore readings if they are more than this amount.
  float m_max_distance = 8.0;
  // If last ping was invalid, this is false.
  bool m_last_ping_ok = false;
  float m_sound_m_per_usec = speedOfSound(20) * kSecPerUsec;
  FloatVariable m_delay_usec;
  FloatVariable m_distance_m;
  HADiscovery* m_ha_discovery = nullptr;
  SingleDependency m_depend;
};

}  // namespace og3
