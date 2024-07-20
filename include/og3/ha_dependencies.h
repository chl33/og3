// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/dependencies.h"

namespace og3 {

class HADiscovery;
class MqttManager;

// HADependencies is a version of Dependencies which makes a module depend on the
/// MQTTManager and HADiscovery, and provides pointers to these modules.
class HADependencies : public Dependencies {
 public:
  bool resolve(const NameToModule& name_to_module) override;
  size_t num_depends_on() const override;
  const Module* depends_on(size_t idx) const override;

  MqttManager* mqtt_manager() { return m_mqtt_manager; }
  const MqttManager* mqtt_manager() const { return m_mqtt_manager; }
  HADiscovery* ha_discovery() { return m_ha_discovery; }
  const HADiscovery* ha_discovery() const { return m_ha_discovery; }

  const bool ok() { return m_mqtt_manager && m_ha_discovery; }

 protected:
  MqttManager* m_mqtt_manager = nullptr;
  HADiscovery* m_ha_discovery = nullptr;
};

template <unsigned N>
class HADependenciesArray : public HADependencies {
 public:
  HADependenciesArray(const std::array<const char*, N>& module_names) : m_more_deps(module_names) {}
  bool resolve(const NameToModule& name_to_module) override;
  size_t num_depends_on() const override {
    return HADependencies::num_depends_on() + m_more_deps.num_depends_on();
  }
  const Module* depends_on(size_t idx) const override {
    const auto d1 = HADependencies::num_depends_on();
    return idx < d1 ? HADependencies::depends_on(idx) : m_more_deps.depends_on(idx - d1);
  }

 protected:
  DependenciesArray<N> m_more_deps;
};

}  // namespace og3
