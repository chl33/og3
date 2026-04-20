// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/variable.h"

namespace og3 {

class WifiManager;

/**
 * @brief Module for synchronizing system time via NTP.
 *
 * This module depends on WifiManager and automatically triggers a time
 * synchronization attempt whenever a WiFi connection is established.
 */
class Ntp : public Module {
 public:
  static const char kName[];

  /** @brief Configuration options for NTP. */
  struct Options {
    const char* server1 = "pool.ntp.org";
    const char* server2 = "time.nist.gov";
    const char* server3 = "time.google.com";
    /** @brief TZ string (e.g., "PST8PDT,M3.2.0,M11.1.0" for Pacific Time). */
    const char* tz_info = "UTC0";
  };

  Ntp(ModuleSystem* module_system, const Options& options);

  /** @return true if time has been successfully synchronized. */
  bool isSynced() const { return m_synced.value(); }

  void updateStatus();

 private:
  void onWifiConnect();

  WifiManager* m_wifi_manager = nullptr;
  VariableGroup m_vg;
  Variable<String> m_server1;
  Variable<String> m_server2;
  Variable<String> m_server3;
  Variable<String> m_tz_info;
  Variable<bool> m_synced;
};

}  // namespace og3
