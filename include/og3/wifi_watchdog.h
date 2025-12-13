// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <chrono>

#include "og3/watchdog.h"

namespace og3 {

// This version of Watchdog will disable the watchdog during OTA firmware updates, so
//  that the firmware update has time to complete.
class WifiWatchdog : public Watchdog {
 public:
  WifiWatchdog(App* app, std::chrono::seconds timeout, std::chrono::milliseconds update);
};

}  // namespace og3
