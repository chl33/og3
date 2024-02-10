// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/app_status.h"
#include "og3/ha_discovery.h"
#include "og3/mqtt_manager.h"
#include "og3/web_app.h"
#include "og3/wifi_monitor.h"

namespace og3 {

// An application designed to work particularly with Home Assistant via MQTT.
class HAApp : public WebApp {
 public:
  struct Options {
    Options(const char* manufacturer_, const char* model_, const WifiApp::Options& app_)
        : app(app_), ha_discovery(manufacturer_, model_, app.software_name) {}
    Options& withMqtt(const MqttManager::Options& mqtt_) {
      this->mqtt = mqtt_;
      return *this;
    }

    WifiApp::Options app;
    MqttManager::Options mqtt;
    HADiscovery::Options ha_discovery;
  };

  explicit HAApp(const Options& options);
  void setup();

  bool mqttSend(const VariableGroup& vg, unsigned flags = VariableBase::kNoPublish) {
    return mqtt_manager().mqttSend(vg, flags);
  }

  MqttManager& mqtt_manager() { return m_mqtt_manager; }
  HADiscovery& ha_discovery() { return m_ha_discovery; }
  AppStatus& app_status() { return m_app_status; }

  void handleMqttConfigRequest(AsyncWebServerRequest* request);
  void handleAppStatusRequest(AsyncWebServerRequest* request);
#ifndef NATIVE
  WebButton createMqttConfigButton();
  WebButton createAppStatusButton();
#endif

 private:
  MqttManager m_mqtt_manager;
  HADiscovery m_ha_discovery;
  AppStatus m_app_status;
  WifiMonitor m_wifi_monitor;
};

}  // namespace og3
