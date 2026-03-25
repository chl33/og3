// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/wifi_monitor.h"

#include "og3/constants.h"
#include "og3/ha_discovery.h"
#include "og3/wifi_manager.h"

namespace og3 {

const char WifiMonitor::kName[] = "wifi-monitor";

WifiMonitor::WifiMonitor(Tasks* tasks)
    : Module(WifiMonitor::kName, tasks->module_system()),
      m_scheduler(10 * kMsecInSec, kMsecInMin, [this]() { statusUpdate(); }, tasks) {
  require(WifiManager::kName, &m_wifi_manager);
  require(MqttManager::kName, &m_mqtt_manager);
  require(HADiscovery::kName, &m_ha_discovery);
  add_init_fn([this]() {
    if (m_ha_discovery && m_wifi_manager) {
      m_ha_discovery->addDiscoveryCallback([this](HADiscovery* had, JsonDocument* json) {
        return had->addMeas(json, m_wifi_manager->rssi(), ha::device_type::kSensor,
                            ha::device_class::sensor::kSignalStrength);
      });
    }
  });
}

void WifiMonitor::statusUpdate() {
  if (!m_mqtt_manager || !m_wifi_manager) {
    return;
  }
  m_wifi_manager->updateStatus();
  m_mqtt_manager->mqttSend(m_wifi_manager->variables());
}

}  // namespace og3
