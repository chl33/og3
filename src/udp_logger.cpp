// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/udp_logger.h"

#include "og3/wifi.h"
#include "og3/wifi_manager.h"

namespace og3 {

UdpLogger::UdpLogger(WifiManager* wifi_mgr, IPAddress ip_address, uint16_t port)
    : m_ip_address(ip_address), m_port(port) {
  wifi_mgr->addConnectCallback([this]() {
#ifdef arduino_arch_esp32
    m_udp.begin(WiFi.localIP(), m_port);
#endif
    m_ready = true;
  });
  wifi_mgr->addDisconnectCallback([this]() { m_ready = false; });
}

void UdpLogger::log(const char* msg) {
#ifndef NATIVE
  if (m_ready) {
    m_udp.beginPacket(m_ip_address, m_port);
#ifdef ARDUINO_ARCH_ESP32
    m_udp.write(reinterpret_cast<const uint8_t*>(msg), strlen(msg));
#elif defined(ARDUINO_ARCH_ESP8266)
    m_udp.write(msg);
#endif
    m_udp.endPacket();
  }
#endif
}

}  // namespace og3
