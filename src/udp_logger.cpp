// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/udp_logger.h"

#include <cstring>

#include "og3/wifi.h"
#include "og3/wifi_manager.h"

namespace og3 {

UdpLogger::UdpLogger(WifiManager* wifi_mgr, IPAddress ip_address, uint16_t port)
    : m_ip_address(ip_address), m_port(port), m_packet_buf(new char[kMaxPacketSize]) {
  if (wifi_mgr) {
    if (wifi_mgr->state() == WifiManager::State::kConnectedSta) {
      m_ready = true;
    }
    wifi_mgr->addConnectCallback([this]() {
#ifndef NATIVE
#ifdef ARDUINO_ARCH_ESP32
      m_udp.begin(WiFi.localIP(), m_port);
#elif defined(ARDUINO_ARCH_ESP8266)
      m_udp.begin(m_port);
#endif
#endif
      m_ready = true;
      flush();
    });

    wifi_mgr->addDisconnectCallback([this]() {
      m_ready = false;
#ifndef NATIVE
      m_udp.stop();
#endif
    });
  }
}

void UdpLogger::send_raw(const char* msg) {
#ifndef NATIVE
  if (!m_ready || !msg) {
    return;
  }
  m_udp.beginPacket(m_ip_address, m_port);
#ifdef ARDUINO_ARCH_ESP32
  m_udp.write(reinterpret_cast<const uint8_t*>(msg), strlen(msg));
#elif defined(ARDUINO_ARCH_ESP8266)
  m_udp.write(msg);
#endif
  m_udp.endPacket();
#endif
}

void UdpLogger::buffer_msg(const char* msg) {
  if (m_buffer.full()) {
    m_buffer.popFront();
    m_dropped_count++;
  }

  BufferedEntry entry;
  entry.timestamp_ms = millis();
  strncpy(entry.text, msg, sizeof(entry.text) - 1);
  entry.text[sizeof(entry.text) - 1] = '\0';
  m_buffer.pushBack(entry);
}

void UdpLogger::log(const char* msg) {
  if (!msg) {
    return;
  }

  // If connected and buffer is empty, send directly
  if (m_ready && m_buffer.empty()) {
    send_raw(msg);
    return;
  }

  // Otherwise queue the message
  buffer_msg(msg);

  // If online, flush pending queue
  if (m_ready) {
    flush();
  }
}

void UdpLogger::flush() {
  if (!m_ready || m_buffer.empty()) {
    return;
  }

  // Notify if any logs were dropped during outage
  if (m_dropped_count > 0) {
    char drop_msg[96];
    snprintf(drop_msg, sizeof(drop_msg),
             "[og3::logger] WARNING: %u log message(s) dropped while offline.",
             static_cast<unsigned>(m_dropped_count));
    send_raw(drop_msg);
    m_dropped_count = 0;
  }

  char* packet_buf = m_packet_buf.get();
  if (!packet_buf) {
    return;
  }
  size_t packet_len = 0;

  while (!m_buffer.empty()) {
    const BufferedEntry& entry = m_buffer.front();
    char line[kMaxEntryLength + 32];
    int line_len = snprintf(line, sizeof(line), "[+%lums] %s\n",
                            static_cast<unsigned long>(entry.timestamp_ms), entry.text);

    if (line_len > 0) {
      // If adding this line exceeds datagram size, send current packet first
      if (packet_len + static_cast<size_t>(line_len) >= kMaxPacketSize) {
        if (packet_len > 0) {
#ifndef NATIVE
          m_udp.beginPacket(m_ip_address, m_port);
          m_udp.write(reinterpret_cast<const uint8_t*>(packet_buf), packet_len);
          m_udp.endPacket();
#endif
          packet_len = 0;
        }
      }

      memcpy(packet_buf + packet_len, line, static_cast<size_t>(line_len));
      packet_len += static_cast<size_t>(line_len);
    }

    m_buffer.popFront();
  }

  // Send trailing batch
  if (packet_len > 0) {
#ifndef NATIVE
    m_udp.beginPacket(m_ip_address, m_port);
    m_udp.write(reinterpret_cast<const uint8_t*>(packet_buf), packet_len);
    m_udp.endPacket();
#endif
  }
}

}  // namespace og3
