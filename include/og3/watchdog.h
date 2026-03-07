// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <chrono>

#include "og3/app.h"
#include "og3/module.h"

namespace og3 {

/**
 * @brief Module for managing the hardware watchdog timer.
 *
 * Watchdog ensures the application remains responsive. If the loop hangs
 * for longer than the timeout period, the device is automatically reset.
 */
class Watchdog : public Module {
 public:
  /**
   * @brief Constructs a Watchdog module.
   * @param app Pointer to the main application.
   * @param timeout Period of inactivity after which the system resets.
   * @param update Frequency at which to "kick" the watchdog.
   */
  Watchdog(App* app, std::chrono::seconds timeout, std::chrono::milliseconds update);

  /** @brief Manually enables the watchdog. */
  void enable();
  /** @brief Manually disables the watchdog. */
  void disable();

 private:
  /** @brief Internal task to periodically reset the hardware timer. */
  void update();

  App* m_app;
  const std::chrono::milliseconds m_update;
};

}  // namespace og3
