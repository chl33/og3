// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <functional>

#include "og3/ha_discovery.h"
#include "og3/module.h"
#include "og3/module_system.h"
#include "og3/variable.h"

namespace og3 {

/**
 * @brief Base class for sensors that measure both Temperature and Humidity.
 *
 * Provides standardized variables and Home Assistant discovery support
 * for dual-property climate sensors (e.g. DHT22, BME280, SHTC3).
 */
class TempHumidity : public Module {
 public:
  /**
   * @brief Constructs a TempHumidity module.
   * @param temp_name Variable name for temperature.
   * @param humidity_name Variable name for humidity.
   * @param module_system_ The ModuleSystem to register with.
   * @param description Human-readable description.
   * @param vg VariableGroup to add variables to.
   * @param publish true to publish via MQTT.
   * @param ha_discovery true to enable Home Assistant discovery.
   */
  TempHumidity(const std::string& temp_name, const std::string& humidity_name,
               ModuleSystem* module_system_, const char* description, VariableGroup& vg,
               bool publish = true, bool ha_discovery = true);

  /** @return true if the last sensor reading was valid. */
  bool ok() const { return m_ok; }
  /** @return Last measured temperature in Celsius. */
  float temperature() const { return m_temperature.value(); }
  /** @return Last measured temperature in Fahrenheit. */
  float temperaturef() const { return 32 + m_temperature.value() * 9 / 5; }
  /** @return Last measured relative humidity (percentage). */
  float humidity() const { return m_humidity.value(); }

  /** @return Constant reference to the temperature variable. */
  const FloatVariable& temperatureVar() const { return m_temperature; }
  /** @return Constant reference to the humidity variable. */
  const FloatVariable& humidityVar() const { return m_humidity; }

 protected:
  /** @brief Marks both variables as failed. */
  bool readingFailed() {
    m_temperature.setFailed();
    m_humidity.setFailed();
    return false;
  }

  bool m_ok = false;
  FloatVariable m_temperature;
  FloatVariable m_humidity;
  HADiscovery* m_ha_discovery = nullptr;
};

}  // namespace og3
