// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#ifndef NATIVE
#include <WiFiUdp.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "og3/compiler_definitions.h"
#include "og3/logger.h"
#include "og3/ring_buffer.h"
#include "og3/wifi.h"

namespace og3 {

class WifiManager;

/**
 * @brief A Logger implementation that sends messages over UDP with offline ring buffering.
 *
 * While WiFi is disconnected, log entries are stored in a fixed-size circular buffer
 * along with a timestamp. When WiFi connects, the queued logs are flushed over UDP
 * in batched datagrams.
 */
class UdpLogger : public Logger {
 public:
  static constexpr uint16_t kDefaultPort = 5005;  ///< @brief Default UDP logging port.
  static constexpr size_t kDefaultCapacity = 32;  ///< @brief Number of buffered log entries.
  static constexpr size_t kMaxEntryLength = 128;  ///< @brief Max length of buffered message.
  static constexpr size_t kMaxPacketSize = 1024;  ///< @brief Max datagram payload during flush.

  /** @brief Entry in the offline ring buffer. */
  struct BufferedEntry {
    uint32_t timestamp_ms;
    char text[kMaxEntryLength];
  };

  /**
   * @brief Constructs a UdpLogger.
   * @param wifi_mgr Pointer to the WiFiManager (used to check connectivity and hook callbacks).
   * @param ip_address Target IP for the logs.
   * @param port Target UDP port.
   */
  UdpLogger(WifiManager* wifi_mgr, IPAddress ip_address, uint16_t port = kDefaultPort);

  /**
   * @brief Logs a message (sends immediately if connected, buffers if offline).
   * @param msg The C-string to log.
   */
  void log(const char* msg) final;

  /** @brief Flushes all buffered log messages over UDP. */
  void flush();

  /** @return Number of messages currently buffered. */
  size_t buffered_count() const { return m_buffer.size(); }

  /** @return Number of messages dropped due to buffer overflow. */
  size_t dropped_count() const { return m_dropped_count; }

  /** @return true if network is ready for UDP transmission. */
  bool is_ready() const { return m_ready; }

 private:
  void send_raw(const char* msg);
  void buffer_msg(const char* msg);

  const IPAddress m_ip_address;
  const uint16_t m_port = 0;
#ifndef NATIVE
  WiFiUDP m_udp;
#endif
  bool m_ready = false;

  RingQueue<BufferedEntry, kDefaultCapacity> m_buffer;
  size_t m_dropped_count = 0;
};

}  // namespace og3
