// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <chrono>

#include "og3/app.h"
#include "og3/module.h"

namespace og3 {

// This module is responsible the ESP32 watchdog
class Watchdog : public Module {
 public:
  Watchdog(App* app, std::chrono::seconds timeout, std::chrono::milliseconds update);

  void enable();
  void disable();

 private:
  void update();

  App* m_app;
  const std::chrono::milliseconds m_update;
};

}  // namespace og3
