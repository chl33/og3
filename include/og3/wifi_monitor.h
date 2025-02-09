// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/ha_dependencies.h"
#include "og3/module.h"
#include "og3/tasks.h"

namespace og3 {

class WifiManager;

class WifiMonitor : public Module {
 public:
  static const char kName[];

  explicit WifiMonitor(Tasks* tasks) OG3_NONNULL();

 private:
  void statusUpdate();

  HADependencies m_dependencies;
  PeriodicTaskScheduler m_scheduler;
  WifiManager* m_wifi_manager = nullptr;
};

}  // namespace og3
