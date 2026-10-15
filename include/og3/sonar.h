// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <math.h>

#include "og3/ha_discovery.h"
#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

/**
 * @brief Computes the speed of sound in air at a given temperature.
 * @param degC Temperature in Celsius.
 * @return Speed of sound in meters per second.
 */
inline float speedOfSound(float degC) { return 331.1 * sqrt(1.0 + degC / 273.15); }

constexpr float kSecPerUsec = 1e-6;  ///< @brief Constant for usec to sec conversion.

/**
 * @brief Module for distance measurement using an ultrasonic sonar sensor (e.g. HC-SR04).
 *
 * This class handles the trigger pulse, timing the echo response, and converting
 * the pulse duration into a distance in meters, with temperature compensation.
 */
class Sonar : public Module {
 public:
  /**
   * @brief Constructs a Sonar module.
   * @param name Unique name for the sensor.
   * @param trigPin GPIO pin for the trigger signal.
   * @param echoPin GPIO pin for the echo signal.
   * @param module_system The ModuleSystem to register with.
   * @param vg The VariableGroup to add distance variables to.
   * @param ha_discovery true to enable Home Assistant discovery.
   */
  Sonar(const std::string& name, int trigPin, int echoPin, ModuleSystem* module_system,
        VariableGroup& vg, bool ha_discovery = true);

  /**
   * @brief Updates the temperature used for speed of sound compensation.
   * @param tempC Current ambient temperature in Celsius.
   */
  void setTemp(float tempC);

  /** @return true if the last measurement was successful. */
  bool ok() const { return m_last_ping_ok; }
  /** @return Measured distance in meters. */
  float distance() const { return m_distance_m.value(); }
  /** @return Measured round-trip pulse duration in microseconds. */
  float ping_usec() const { return m_delay_usec.value(); }

  /** @brief Performs a full ping cycle (triggers and reads). */
  void read();

  /** @brief Triggers the ultrasonic pulse. */
  void ping();

 private:
  bool haDeclare(HADiscovery* had, JsonObject json);

  const uint8_t m_trigPin;
  const uint8_t m_echoPin;
  const String m_delay_name;
  float m_max_distance = 8.0;  ///< @brief Maximum distance to consider valid.
  bool m_last_ping_ok = false;
  float m_sound_m_per_usec = speedOfSound(20) * kSecPerUsec;
  FloatVariable m_delay_usec;
  FloatVariable m_distance_m;
  HADiscovery* m_ha_discovery = nullptr;
};

}  // namespace og3
