// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/app_status.h"
#include "og3/ha_discovery.h"
#include "og3/mqtt_manager.h"
#include "og3/web_app.h"
#include "og3/wifi_monitor.h"

namespace og3 {

/**
 * @brief An application class specifically tailored for Home Assistant integration via MQTT.
 *
 * HAApp extends WebApp by adding MqttManager, HADiscovery, and AppStatus modules.
 * it automatically handles Home Assistant discovery messages and provides status
 * reporting compatible with HA.
 */
class HAApp : public WebApp {
 public:
  HAApp(const HAApp&) = delete;
  HAApp(HAApp&&) = delete;
  HAApp& operator=(const HAApp&) = delete;
  HAApp& operator=(HAApp&&) = delete;

  /** @brief Configuration options for the HAApp. */
  struct Options {
    /**
     * @brief Constructs HAApp options.
     * @param manufacturer_ Device manufacturer name.
     * @param model_ Device model name.
     * @param app_ Underlying WifiApp options.
     */
    Options(const char* manufacturer_, const char* model_, const WifiApp::Options& app_)
        : app(app_), ha_discovery(manufacturer_, model_, app.software_name) {}

    /**
     * @brief Configures MQTT settings.
     * @param mqtt_ MqttManager options.
     * @return Reference to this Options object.
     */
    Options& withMqtt(const MqttManager::Options& mqtt_) {
      this->mqtt = mqtt_;
      return *this;
    }

    WifiApp::Options app;               ///< Wifi/Base application options.
    MqttManager::Options mqtt;          ///< MQTT connection options.
    HADiscovery::Options ha_discovery;  ///< Home Assistant discovery options.
  };

  /** @brief Constructs an HAApp instance. */
  explicit HAApp(const Options& options);

  /** @brief Performs application setup, including MQTT and discovery. */
  void setup();

  /**
   * @brief Publishes a group of variables to MQTT.
   * @param vg The VariableGroup to publish.
   * @param flags Filter flags for variable selection.
   * @return true if publishing was successful.
   */
  bool mqttSend(const VariableGroup& vg, unsigned flags = VariableBase::kNoPublish) {
    return mqtt_manager().mqttSend(vg, flags);
  }

  /** @return Reference to the MQTT manager. */
  MqttManager& mqtt_manager() { return m_mqtt_manager; }
  /** @return Reference to the Home Assistant discovery manager. */
  HADiscovery& ha_discovery() { return m_ha_discovery; }
  /** @return Reference to the AppStatus module. */
  AppStatus& app_status() { return m_app_status; }

  /** @brief Web handler for the MQTT configuration page. */
  NetHandlerStatus handleMqttConfigRequest(NetRequest* request, NetResponse* response);
  /** @brief Web handler for the application status page. */
  NetHandlerStatus handleAppStatusRequest(NetRequest* request, NetResponse* response);

#ifndef NATIVE
  /** @return A button that links to the MQTT config page. */
  WebButton createMqttConfigButton();
  /** @return A button that links to the App Status page. */
  WebButton createAppStatusButton();
#endif

 private:
  MqttManager m_mqtt_manager;
  HADiscovery m_ha_discovery;
  AppStatus m_app_status;
  WifiMonitor m_wifi_monitor;
};

}  // namespace og3
