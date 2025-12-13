// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/wifi_watchdog.h"

#include "og3/ota_manager.h"

namespace og3 {

WifiWatchdog::WifiWatchdog(App* app, std::chrono::seconds timeout, std::chrono::milliseconds update)
    : Watchdog(app, timeout, update) {
#ifndef NATIVE
  // If we are part of a WifiApp, we should disable the watchdog during OTA updates.
  add_link_fn([this](NameToModule& name_to_module) -> bool {
    auto* ota = OtaManager::get(name_to_module);
    if (ota) {
      ota->addOtaStartCallback([this]() { disable(); });
      ota->addOtaEndCallback([this]() { enable(); });
      ota->addOtaErrorCallback([this](int) { enable(); });
    }
    return true;
  });
#endif
}

}  // namespace og3
