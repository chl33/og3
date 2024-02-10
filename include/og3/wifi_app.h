// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <memory>

#include "og3/app.h"
#include "og3/config_interface.h"
#include "og3/flash_support.h"
#include "og3/mdns.h"
#include "og3/ota_manager.h"
#include "og3/wifi.h"
#include "og3/wifi_manager.h"

namespace og3 {

class WifiManager;

// An application which supports Wifi.
// The WifiManager manages the wifi connection, OtaManager supports firmware update,
//  and mDNS broadcasts the board/device name.
class WifiApp : public App {
 public:
  struct Options {
    App::Options app;
    const char* default_device_name;
    const char* software_name;
    WifiManager::Options wifi;
    OtaManager::Options ota;
    IPAddress udp_log_host;
    Options& withDefaultDeviceName(const char* default_device_name) {
      this->default_device_name = default_device_name;
      return *this;
    }
    Options& withSoftwareName(const char* software_name) {
      this->software_name = software_name;
      return *this;
    }
    Options& withApp(const App::Options& app) {
      this->app = app;
      return *this;
    }
    Options& withWifi(const WifiManager::Options& wifi) {
      this->wifi = wifi;
      return *this;
    }
    Options& withOta(const OtaManager::Options& ota) {
      this->ota = ota;
      return *this;
    }
    Options& withUdpLogHost(const IPAddress& val) {
      this->udp_log_host = val;
      return *this;
    }
  };

  explicit WifiApp(const Options& options);

  WifiManager& wifi_manager() { return m_wifi_manager; }
  const WifiManager& wifi_manager() const { return m_wifi_manager; }
  ConfigInterface& config() { return m_config; }

  const String& board_name() const { return wifi_manager().board(); }
  const char* board_cname() const { return board_name().c_str(); }
  const char* software_name() const { return m_software_name; }

 protected:
  const char* m_software_name;
  ConfigInterface m_config;
  FlashSupport m_flash_support;
  WifiManager m_wifi_manager;
  OtaManager m_ota_manager;
  Mdns m_dns;
  std::unique_ptr<Logger> m_ptr_logger;
};

}  // namespace og3
