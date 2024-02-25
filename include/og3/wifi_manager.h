// Copyright (c) 2024 Chris Lee and contibuters.
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
// if status parameter is not given, the value of WiFi.status() is used.
const char* strWifiStatus(wl_status_t status = kWiFiStatusNone);
#endif

// A WifiManager tries to connect to an ESSID if one is configured.
// If not configured, or if connecting to the specified ESSID fails, then
//  it sets itself up as an soft AP point using the board/device name.
// The name of the board/device, the ESSID, and the wifi password are
//  stored in a configuration in flash storage, and can be configured
//  via a web form.
class WifiManager : public Module {
 public:
  static const char kName[];
  static const char kConfigUrl[];

  struct Options {
    Options() {}
    const char* default_essid = nullptr;
    const char* default_password = nullptr;
  };

  WifiManager(const char* default_board_name, Tasks* tasks, const Options& options = Options())
      OG3_NONNULL();

  void set_enable(bool enable) { m_enable = enable; }

  const String& board() const { return m_board.value(); }
  const String& essid() const { return m_essid.value(); }
  const String& password() const { return m_password.value(); }
  bool apMode() const { return m_ap_mode; }

  const Variable<String>& ipAddressVariable() const { return m_ip_addr; }
  const String& ipAddress() const { return m_ip_addr.value(); }
  bool wasConnected() const { return m_was_connected; }

  const VariableGroup& variables() const { return m_vg; }
  VariableGroup& mutableVariables() { return m_vg; }
  const Variable<int>& rssi() const { return m_rssi; }

  void addConnectCallback(const std::function<void()>& callback) {
    m_connectCallbacks.push_back(callback);
  }
  void addDisconnectCallback(const std::function<void()>& callback) {
    m_disconnectCallbacks.push_back(callback);
  }
  void addSoftAPCallback(const std::function<void()>& callback) {
    m_softAPCallbacks.push_back(callback);
  }

  static WifiManager* get(const NameToModule& n2m) { return GetModule<WifiManager>(n2m, kName); }

  void updateStatus();

 private:
  void trySetup();
  void onConnect();
  void onDisconnect();

#ifdef ARDUINO_ARCH_ESP32
  void onWifiEvent(arduino_event_id_t task, arduino_event_info_t info);
#endif

#ifndef NATIVE
  std::unique_ptr<DNSServer> m_dns_server;
#endif
  SingleDependency m_dependencies;
  TaskIdScheduler m_scheduler;
  VariableGroup m_vg;
  // Config varibles
  Variable<String> m_board;
  Variable<String> m_essid;
  Variable<String> m_password;
  // Output variables
  Variable<String> m_ip_addr;
  Variable<int> m_rssi;
  bool m_was_connected = false;
  unsigned long m_start_connect_msec = 0;
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
