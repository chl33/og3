// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/tasks.h"

namespace og3 {

class WifiManager;

/**
 * @brief Module providing multicast DNS (mDNS) support.
 *
 * Allows the device to be reachable via a local hostname (e.g. `hostname.local`)
 * rather than just an IP address.
 */
class Mdns : public Module {
 public:
  static const char kName[];  ///< @brief "mdns"

  /** @brief Constructs an Mdns module. */
  explicit Mdns(Tasks* tasks);

 private:
  void connect();

  Tasks* const m_tasks;
  WifiManager* m_wifi_manager = nullptr;
  bool m_ok = false;
  bool m_connected = false;
};

}  // namespace og3
