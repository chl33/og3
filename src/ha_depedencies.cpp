// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/ha_dependencies.h"
#include "og3/ha_discovery.h"
#include "og3/mqtt_manager.h"

namespace og3 {

bool HADependencies::resolve(const NameToModule& name_to_module) {
  m_mqtt_manager = MqttManager::get(name_to_module);
  m_ha_discovery = HADiscovery::get(name_to_module);
  return m_mqtt_manager && m_ha_discovery;
}
size_t HADependencies::num_depends_on() const { return 2; }

const Module* HADependencies::depends_on(size_t idx) const {
  switch (idx) {
    case 0:
      return m_mqtt_manager;
    case 1:
      return m_ha_discovery;
  }
  return nullptr;
}

}  // namespace og3
