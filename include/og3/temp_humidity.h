// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <functional>

#include "og3/module_system.h"
#include "og3/ha_discovery.h"
#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

// TempHumidity is a wrapper for a sensor which measures temperature and humidity.
// Subclasses can support different specific sensors.
// Values can be logged to HomeAssistant via MQTT.
class TempHumidity : public Module {
 public:
  TempHumidity(const char* temp_name, const char* humidity_name, ModuleSystem* module_system_,
               const char* description, VariableGroup* vg, bool publish = true,
               bool ha_discovery = true);

  bool ok() const { return m_ok; }
  float temperature() const { return m_temperature.value(); }
  float humidity() const { return m_humidity.value(); }
  const FloatVariable& temperatureVar() const { return m_temperature; }
  const FloatVariable& humidityVar() const { return m_humidity; }

 protected:
  // Depend of HA Discovery if we are using it.
  bool readingFailed() {
    m_temperature.setFailed();
    m_humidity.setFailed();
    return false;
  }

  bool m_ok = false;
  FloatVariable m_temperature;
  FloatVariable m_humidity;
  HADiscovery* m_ha_discovery = nullptr;
  SingleDependency m_depend;
};

}  // namespace og3
