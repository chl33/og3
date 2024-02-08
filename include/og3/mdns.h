// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/tasks.h"

namespace og3 {

class WifiManager;

// A module handles mDNS support for an application.
class Mdns : public Module {
 public:
  static const char kName[];

  explicit Mdns(Tasks* tasks);

 private:
  void connect();

  Tasks* const m_tasks;
  WifiManager* m_wifi_manager = nullptr;
  SingleDependency m_dependency;
  bool m_ok = false;
  bool m_connected = false;
};

}  // namespace og3
