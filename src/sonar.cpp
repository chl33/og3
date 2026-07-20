// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/sonar.h"

#include "og3/units.h"

namespace og3 {

// m/s (dry air)
Sonar::Sonar(const std::string& name, int trigPin, int echoPin, ModuleSystem* module_system,
             VariableGroup& vg, bool ha_discovery)
    : Module(name, module_system),
      m_trigPin(trigPin),
      m_echoPin(echoPin),
      m_delay_name(String(name.c_str()) + "_usec"),
      m_delay_usec(m_delay_name.c_str(), 0.0f, units::kMicroseconds, "sonar ping time", 0, 0, vg),
      m_distance_m(name.c_str(), 0.0f, units::kMeters, "measured distance", 0, 2, vg) {
  if (ha_discovery) {
    require(HADiscovery::kName, &m_ha_discovery);
  }
  add_init_fn([this]() {
    pinMode(m_trigPin, OUTPUT);
    pinMode(m_echoPin, INPUT);
    return true;
  });
  add_init_fn([this]() {
    if (m_ha_discovery) {
      m_ha_discovery->addDiscoveryCallback([this](HADiscovery* had, JsonDocument* json) {
        // had->addIcon("mdi:signal-distance-variant", json);
        return had->addMeas(json, m_distance_m, ha::device_type::kSensor,
                            ha::device_class::sensor::kDistance);
      });
      m_ha_discovery->addDiscoveryCallback([this](HADiscovery* had, JsonDocument* json) {
        // had->addIcon("mdi:timer-outline", json);
        return had->addMeas(json, m_delay_usec, ha::device_type::kSensor,
                            ha::device_class::sensor::kDuration);
      });
    }
  });
}

void Sonar::setTemp(float tempC) { m_sound_m_per_usec = speedOfSound(tempC) * kSecPerUsec; }

void Sonar::read() { ping(); }

void Sonar::ping() {
  digitalWrite(m_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(m_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(m_trigPin, LOW);
  m_delay_usec = pulseIn(m_echoPin, HIGH);
  // Divide distance by 2 because sound travels there and back.
  m_distance_m = (m_sound_m_per_usec * m_delay_usec.value()) / 2;
  m_last_ping_ok = (m_distance_m.value() <= m_max_distance);
  if (!m_last_ping_ok) {
    m_distance_m.setFailed();
  }
}

}  // namespace og3
