// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#ifndef NATIVE
#include <WiFiUdp.h>
#endif

#include "og3/logger.h"
#include "og3/wifi.h"

namespace og3 {

class WifiManager;

/**
 * @brief A Logger implementation that sends messages over UDP.
 *
 * This allows remote log monitoring by sending plain-text UDP frames to a
 * specified IP address and port.
 */
class UdpLogger : public Logger {
 public:
  static constexpr uint16_t kDefaultPort = 5005;  ///< @brief Default UDP logging port.

  /**
   * @brief Constructs a UdpLogger.
   * @param wifi_mgr Pointer to the WiFiManager (used to check connectivity).
   * @param ip_address Target IP for the logs.
   * @param port Target UDP port.
   */
  UdpLogger(WifiManager* wifi_mgr, IPAddress ip_address, uint16_t port = kDefaultPort);

  /**
   * @brief Logs a message via UDP.
   * @param msg The C-string to log.
   */
  void log(const char* msg) final;

  /** @brief Updates the logger when WiFi becomes available. */
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
