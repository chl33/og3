// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/mdns.h"

#include "og3/wifi_manager.h"

#ifdef ARDUINO_ARCH_ESP32
#include <ESPmDNS.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266mDNS.h>
#endif

namespace og3 {

const char Mdns::kName[] = "mdns";

// Wrapper for digital input.
Mdns::Mdns(Tasks* tasks)
    : Module(kName, tasks->module_system()), m_tasks(tasks), m_dependency(WifiManager::kName) {
  setDependencies(&m_dependency);
#ifndef NATIVE
  // Module callbacks
  add_link_fn([this](NameToModule& name_to_module) -> bool {
    m_wifi_manager = WifiManager::get(name_to_module);
    return !!m_wifi_manager;
  });
  add_init_fn([this]() {
    if (m_wifi_manager) {
      m_wifi_manager->addConnectCallback([this] { connect(); });
      m_wifi_manager->addDisconnectCallback([this] { m_connected = false; });
    }
  });
#if defined(ARDUINO_ARCH_ESP8266)
  add_update_fn([this]() {
    if (m_ok && m_connected) {
      MDNS.update();
    }
  });
#endif
#endif
}

void Mdns::connect() {
#ifndef NATIVE
  m_connected = true;
  if (m_ok) {
    return;
  }
  const auto& hostname = m_wifi_manager->board();
  if (!MDNS.begin(hostname.c_str())) {  // Start the mDNS responder
    log()->log("Error setting up MDNS responder!");
    m_tasks->runIn(5 * kMsecInSec, [this]() { connect(); });
    return;
  }
  log()->logf("mDNS initialized to %s.\n", hostname.c_str());
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  m_ok = true;
#endif
}

}  // namespace og3
