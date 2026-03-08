// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#ifndef NATIVE
#include <DNSServer.h>
#endif

#include <functional>
#include <memory>

#include "og3/compiler_definitions.h"
#include "og3/constants.h"
#include "og3/logger.h"
#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/variable.h"
#include "og3/wifi.h"

namespace og3 {

class ConfigInterface;

#ifndef NATIVE
static constexpr wl_status_t kWiFiStatusNone = static_cast<wl_status_t>(-1);
/**
 * @brief Converts a wl_status_t code to a human-readable string.
 * @param status The status code (defaults to current WiFi status).
 * @return Descriptive C-string.
 */
const char* strWifiStatus(wl_status_t status = kWiFiStatusNone);
#endif

/**
 * @brief Manages WiFi connectivity, including client mode and Soft AP failover.
 *
 * WifiManager attempts to connect to a configured ESSID. If no ESSID is set or
 * the connection fails, it starts a captive portal Soft AP. It manages
 * persistence of WiFi credentials and provides callbacks for connection state changes.
 */
class WifiManager : public Module {
 public:
  static const char kName[];       ///< @brief "wifi"
  static const char kConfigUrl[];  ///< @brief "/wifi"

  static constexpr unsigned long kInitialRetryDelayMs = 5000;  ///< @brief Starting retry backoff.
  static constexpr unsigned long kMaxRetryDelayMs = 60000;     ///< @brief Maximum retry delay.
  static constexpr unsigned long kBackoffMultiplier = 2;       ///< @brief Backoff exponent.

  /** @brief Configuration options for WiFi. */
  struct Options {
    Options() {}
    const char* default_essid = nullptr;
    const char* default_password = nullptr;
    const char* ap_password = nullptr;  ///< Password for the fall-back Soft AP.
    /** @brief Sets the Soft AP password. */
    Options& withApPassword(const char* pw) {
      this->ap_password = pw;
      return *this;
    }
  };

  /** @brief Constructs a WifiManager. */
  WifiManager(const char* default_board_name, Tasks* tasks, const Options& options = Options());

  /** @brief Manually enables or disables WiFi. */
  void set_enable(bool enable) { m_enable = enable; }

  /** @return The configured board name (hostname). */
  const String& board() const { return m_board.value(); }
  /** @return The target ESSID. */
  const String& essid() const { return m_essid.value(); }
  /** @return The WiFi password. */
  const String& password() const { return m_password.value(); }
  /** @return true if currently running in Soft AP mode. */
  bool apMode() const { return m_ap_mode; }

  /** @return Variable tracking the current IP address. */
  const Variable<String>& ipAddressVariable() const { return m_ip_addr; }
  /** @return Current IP address as a string. */
  const String& ipAddress() const { return m_ip_addr.value(); }
  /** @return true if at least one successful connection was made this boot. */
  bool wasConnected() const { return m_was_connected; }

  /** @return Reference to the variable group for WiFi settings. */
  const VariableGroup& variables() const { return m_vg; }
  /** @return Reference to the variable group for WiFi settings. */
  VariableGroup& variables() { return m_vg; }
  /** @return Variable tracking signal strength (RSSI). */
  const Variable<int>& rssi() const { return m_rssi; }
  /** @return Current Soft AP password. */
  const char* ap_password() const { return m_ap_password; }

  /** @brief Registers a callback for when WiFi client connects. */
  void addConnectCallback(const std::function<void()>& callback) {
    m_connectCallbacks.push_back(callback);
  }
  /** @brief Registers a callback for when WiFi client disconnects. */
  void addDisconnectCallback(const std::function<void()>& callback) {
    m_disconnectCallbacks.push_back(callback);
  }
  /** @brief Registers a callback for when Soft AP starts. */
  void addSoftAPCallback(const std::function<void()>& callback) {
    m_softAPCallbacks.push_back(callback);
  }

  /** @return Pointer to the WifiManager instance. */
  static WifiManager* get(const NameToModule& n2m) { return GetModule<WifiManager>(n2m, kName); }

  /** @brief Periodically checks and updates WiFi status variables. */
  void updateStatus();

 protected:
  /** @brief Internal handler for successful connection. */
  void onConnect();
  /** @brief Internal handler for connection loss. */
  void onDisconnect();
  unsigned long m_retry_delay_ms = kInitialRetryDelayMs;

 private:
  void trySetup();
  void startAp();
  void startClient();
  void scheduleSanityCheck();
  void sanityCheck();

#ifdef ARDUINO_ARCH_ESP32
  void onWifiEvent(arduino_event_id_t task, arduino_event_info_t info);
#endif

#ifndef NATIVE
  std::unique_ptr<DNSServer> m_dns_server;
#endif
  TaskIdScheduler m_scheduler;
  TaskIdScheduler m_sanity_scheduler;
  const char* m_ap_password;
  VariableGroup m_vg;
  Variable<String> m_board;
  Variable<String> m_essid;
  Variable<String> m_password;
  Variable<String> m_ip_addr;
  Variable<int> m_rssi;
  bool m_was_connected = false;
  unsigned long m_start_connect_msec = 0;
  unsigned long m_disconnect_msec = 0;
  ConfigInterface* m_config = nullptr;
  bool m_enable = true;

  std::vector<std::function<void()>> m_connectCallbacks;
  std::vector<std::function<void()>> m_disconnectCallbacks;
  std::vector<std::function<void()>> m_softAPCallbacks;
#ifdef ARDUINO_ARCH_ESP32
  WiFiEventId_t m_wifiEventIdConnected;
  WiFiEventId_t m_wifiEventIdDisconnected;
#elif defined(ARDUINO_ARCH_ESP8266)
  WiFiEventHandler m_wifiConnectHandler;
  WiFiEventHandler m_wifiDisconnectHandler;
#endif

  bool m_ap_mode = false;
};

}  // namespace og3
