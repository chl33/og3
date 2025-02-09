// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/temp_humidity.h"

#include "og3/units.h"

namespace og3 {

TempHumidity::TempHumidity(const char* temp_name, const char* humidity_name,
                           ModuleSystem* module_system_, const char* description, VariableGroup& vg,
                           bool publish, bool ha_discovery)
    : Module(temp_name, module_system_),
      m_temperature(temp_name, 0.0f, units::kCelsius, description,
                    publish ? 0 : VariableBase::kNoPublish, 1, vg),
      m_humidity(humidity_name, 0.0f, units::kPercentage, humidity_name,
                 publish ? 0 : VariableBase::kNoPublish, 0, vg),
      m_depend(ha_discovery ? HADiscovery::kName : nullptr) {
  m_temperature.setFailed();
  m_humidity.setFailed();
  setDependencies(&m_depend);
  if (ha_discovery) {
    add_link_fn([this](NameToModule& name_to_module) -> bool {
      m_ha_discovery = HADiscovery::get(name_to_module);
      return true;
    });
  }
  add_init_fn([this]() {
    if (m_ha_discovery) {
      m_ha_discovery->addDiscoveryCallback([this](HADiscovery* had, JsonDocument* json) {
        return had->addMeas(json, m_temperature, ha::device_type::kSensor,
                            ha::device_class::sensor::kTemperature);
      });
      m_ha_discovery->addDiscoveryCallback([this](HADiscovery* had, JsonDocument* json) {
        return had->addMeas(json, m_humidity, ha::device_type::kSensor,
                            ha::device_class::sensor::kHumidity);
      });
    }
  });
}

}  // namespace og3
