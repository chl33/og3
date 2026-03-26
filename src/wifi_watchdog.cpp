// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/wifi_watchdog.h"

#include "og3/ota_manager.h"

namespace og3 {

WifiWatchdog::WifiWatchdog(App* app, std::chrono::seconds timeout, std::chrono::milliseconds update)
    : Watchdog(app, timeout, update) {
#ifndef NATIVE
  // If we are part of a WifiApp, we should disable the watchdog during OTA updates.
  require(OtaManager::kName, &m_ota_manager);
  add_init_fn([this]() {
    if (m_ota_manager) {
      m_ota_manager->addOtaStartCallback([this]() { disable(); });
      m_ota_manager->addOtaEndCallback([this]() { enable(); });
      m_ota_manager->addOtaErrorCallback([this](int) { enable(); });
    }
  });
#endif
}

}  // namespace og3
