// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/dependencies.h"

namespace og3 {

class HADiscovery;
class MqttManager;

/**
 * @brief Dependency manager for modules requiring MQTT and Home Assistant discovery.
 *
 * This class ensures that a module's dependencies on MqttManager and HADiscovery
 * are correctly resolved by the ModuleSystem.
 */
class HADependencies : public Dependencies {
 public:
  /**
   * @brief Resolves MqttManager and HADiscovery from the module map.
   * @param name_to_module Map of names to module pointers.
   * @return true if both required modules were found.
   */
  bool resolve(const NameToModule& name_to_module) override;

  /** @return 2 (MqttManager + HADiscovery). */
  size_t num_depends_on() const override;

  /** @brief Returns pointer to MqttManager (0) or HADiscovery (1). */
  const Module* depends_on(size_t idx) const override;

  /** @return Pointer to the resolved MqttManager. */
  MqttManager* mqtt_manager() { return m_mqtt_manager; }
  /** @return Constant pointer to the resolved MqttManager. */
  const MqttManager* mqtt_manager() const { return m_mqtt_manager; }
  /** @return Pointer to the resolved HADiscovery. */
  HADiscovery* ha_discovery() { return m_ha_discovery; }
  /** @return Constant pointer to the resolved HADiscovery. */
  const HADiscovery* ha_discovery() const { return m_ha_discovery; }

  /** @return true if both dependencies were successfully resolved. */
  const bool ok() { return m_mqtt_manager && m_ha_discovery; }

 protected:
  MqttManager* m_mqtt_manager = nullptr;
  HADiscovery* m_ha_discovery = nullptr;
};

/**
 * @brief Extends HADependencies with an additional fixed-size array of dependencies.
 * @tparam N The number of additional dependencies.
 */
template <unsigned N>
class HADependenciesArray : public HADependencies {
 public:
  /**
   * @brief Constructs HADependenciesArray.
   * @param module_names Array of additional module names to depend on.
   */
  HADependenciesArray(const std::array<const char*, N>& module_names) : m_more_deps(module_names) {}

  /** @return Total number of dependencies (2 + N). */
  size_t num_depends_on() const override {
    return HADependencies::num_depends_on() + m_more_deps.num_depends_on();
  }

  /** @return Pointer to a dependency by index. */
  const Module* depends_on(size_t idx) const override {
    const auto d1 = HADependencies::num_depends_on();
    return idx < d1 ? HADependencies::depends_on(idx) : m_more_deps.depends_on(idx - d1);
  }

 protected:
  DependenciesArray<N> m_more_deps;
};

}  // namespace og3
