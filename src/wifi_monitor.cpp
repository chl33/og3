// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/wifi_monitor.h"

#include "og3/constants.h"
#include "og3/ha_discovery.h"
#include "og3/wifi_manager.h"

namespace og3 {

const char WifiMonitor::kName[] = "wifi-monitor";

WifiMonitor::WifiMonitor(Tasks* tasks)
    : Module(WifiMonitor::kName, tasks->module_system()),
      m_scheduler(
          10 * kMsecInSec, kMsecInMin, [this]() { statusUpdate(); }, tasks) {
  setDependencies(&m_dependencies);
  add_link_fn([this](NameToModule& name_to_module) -> bool {
    m_wifi_manager = WifiManager::get(name_to_module);
    return m_wifi_manager != nullptr;
  });
  add_init_fn([this]() {
    if (m_dependencies.ok() && m_wifi_manager) {
      m_dependencies.ha_discovery()->addDiscoveryCallback(
          [this](HADiscovery* had, JsonDocument* json) {
            return had->addMeas(json, m_wifi_manager->rssi(), ha::device_type::kSensor,
                                ha::device_class::sensor::kSignalStrength);
          });
    }
  });
}

void WifiMonitor::statusUpdate() {
  if (!m_dependencies.mqtt_manager() || !m_wifi_manager) {
    return;
  }
  log()->log("WifiMonitor::statusUpdate()");
  m_wifi_manager->updateStatus();
  m_dependencies.mqtt_manager()->mqttSend(m_wifi_manager->variables());
}

}  // namespace og3
