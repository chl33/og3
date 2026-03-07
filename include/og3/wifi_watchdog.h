// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <chrono>

#include "og3/watchdog.h"

namespace og3 {

/**
 * @brief A specialized Watchdog that handles network-aware constraints.
 *
 * WifiWatchdog automatically disables the hardware watchdog timer during
 * Over-The-Air (OTA) updates to prevent resets during long firmware writes.
 */
class WifiWatchdog : public Watchdog {
 public:
  /** @brief Constructs a WifiWatchdog. */
  WifiWatchdog(App* app, std::chrono::seconds timeout, std::chrono::milliseconds update);
};

}  // namespace og3
