// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>
#ifndef NATIVE
#include <WiFiUdp.h>
#endif
#include <cstdint>

#include "og3/logger.h"
#include "og3/wifi.h"

namespace og3 {

class WifiManager;

// A logger which simply sends logged text in plain UDP frames to a specified enpoint.
class UdpLogger : public Logger {
 public:
  static constexpr uint16_t kDefaultPort = 5005;

  UdpLogger(WifiManager* wifi_mgr, IPAddress ip_address, uint16_t port = kDefaultPort);
  void log(const char* msg) final;

  void set_wifi_manager();

 private:
  const IPAddress m_ip_address;
  const unsigned m_port = 0;
#ifndef NATIVE
  WiFiUDP m_udp;
#endif
  bool m_ready = false;
};

}  // namespace og3
