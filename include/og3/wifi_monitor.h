// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/ha_dependencies.h"
#include "og3/module.h"
#include "og3/tasks.h"

namespace og3 {

class WifiManager;

/**
 * @brief Module for periodic monitoring of WiFi signal strength.
 *
 * WifiMonitor periodically polls the WiFi signal level (RSSI) and
 * updates associated variables for telemetry and Home Assistant.
 */
class WifiMonitor : public Module {
 public:
  static const char kName[];  ///< @brief "wifi_monitor"

  /** @brief Constructs a WifiMonitor. */
  explicit WifiMonitor(Tasks* tasks);

 private:
  /** @brief Scheduled task to refresh signal metrics. */
  void statusUpdate();

  HADependencies m_dependencies;
  PeriodicTaskScheduler m_scheduler;
  WifiManager* m_wifi_manager = nullptr;
};

}  // namespace og3
