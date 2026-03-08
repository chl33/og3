// Copyright (c) 2026 Chris Lee and contibuters.
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

/**
 * @brief Base class for applications requiring WiFi and standard network services.
 *
 * WifiApp coordinates several core modules:
 * - WifiManager: Connectivity and configuration.
 * - OtaManager: Over-The-Air updates.
 * - Mdns: Local hostname resolution.
 * - FlashSupport: Configuration persistence.
 *
 * It automatically handles setup, loading configuration from flash, and logging
 * redirection.
 */
class WifiApp : public App {
 public:
  /** @brief Comprehensive options for initializing all core WifiApp services. */
  struct Options {
    App::Options app;
    const char* default_device_name;  ///< Initial hostname/SSID.
    const char* software_name;        ///< Software version/identifier.
    WifiManager::Options wifi;        ///< WiFi-specific tuning.
    OtaManager::Options ota;          ///< OTA settings.
    IPAddress udp_log_host;           ///< Optional remote IP for UDP logging.

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

  /** @brief Constructs a WifiApp. */
  explicit WifiApp(const Options& options);

  /** @return Reference to the WiFi manager. */
  WifiManager& wifi_manager() { return m_wifi_manager; }
  /** @return Constant reference to the WiFi manager. */
  const WifiManager& wifi_manager() const { return m_wifi_manager; }
  /** @return Reference to the persistent configuration interface. */
  ConfigInterface& config() { return m_config; }

  /** @return The configured board name. */
  const String& board_name() const { return wifi_manager().board(); }
  /** @return The configured board name as a C-string. */
  const char* board_cname() const { return board_name().c_str(); }
  /** @return The name of the software running on the board. */
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
