// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>

#include <cstdint>
#ifndef NATIVE
#include <AsyncMqttClient.h>
#endif

#include <functional>

#include "og3/logger.h"
#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/variable.h"
#include "og3/wifi.h"
#include "og3/wifi_manager.h"

namespace og3 {

// TODO: Why can broker port not be set?

// MqttManager works with a WiFiManager to establish and maintain a connection to a remote
//  MQTT server.
// When the WiFi connection is established, MqttManager attempts to connect to the MQTT server.
// When the MQTT connection is established, MqttManager sends "online" to the will_topic,
//  and it registers a will that switches the value of "offline" when this client is disconnected.
class MqttManager : public Module {
 public:
  static const char kName[];
  static const char kConfigUrl[];

  static MqttManager* get(const NameToModule& n2m) { return GetModule<MqttManager>(n2m, kName); }

  enum class Mode {
    kHomeAssistant,
    kAdafruitIO,
  };
  struct Options {
    Options() {}

    Options& withAppDomain(const char* val) {
      this->app_domain = val;
      return *this;
    }
    const char* app_domain = "og3";
    const char* default_server = nullptr;
    const char* default_user = nullptr;
    const char* default_password = nullptr;
    const char* default_will_topic = nullptr;
    uint16_t port = 1883;
    int will_qos = 1;
    bool will_retain = true;
    Mode mode = Mode::kHomeAssistant;
  };
  MqttManager(const Options& opts, Tasks* tasks);

  void connect();

  void addConnectCallback(const std::function<void()>& callback) {
    m_connectCallbacks.push_back(callback);
  }
  void addDisconnectCallback(const std::function<void()>& callback) {
    m_disconnectCallbacks.push_back(callback);
  }
#ifndef NATIVE
  AsyncMqttClient& client() { return m_mqttClient; }
  const AsyncMqttClient& client() const { return m_mqttClient; }
#endif

  bool connected() const {
#ifndef NATIVE
    return client().connected();
#else
    return true;
#endif
  }

  void mqttSend(const char topic[], const char content[]);
  bool mqttSend(const VariableGroup& variables, unsigned flags = VariableBase::kNoPublish);
  // arguments: (const char* topic, const char* payload, size_t len) {
  using MqttMsgCallbackFn = std::function<void(const char*, const char*, size_t)>;
  void subscribe(const String& topic, const MqttMsgCallbackFn& fn);

  String boardTopic(const char* device_name = nullptr) const;
  String topic(const char* name, const char* device_name = nullptr) const;
  String willTopic(const char* device_name = nullptr) const;

  // Accessors.
  const char* board() const {
    return m_wifi_manager ? m_wifi_manager->board().c_str() : "og3board";
  }
  const String& host() const { return m_host_addr.value(); }
  uint16_t port() const { return m_opts.port; }
  const String& auth_user() const { return m_auth_user.value(); }
  const String& auth_password() const { return m_auth_password.value(); }
  Mode mode() const { return m_mode.value(); }
  // const Options& options() const { return m_opts; }

  const VariableGroup& variables() const { return m_vg; }
  VariableGroup& variables() { return m_vg; }
  enum ConnectionStatus { kNotConnected, kConnected };
  const EnumStrVariable<ConnectionStatus>& connectionStatusVariable() const { return m_connected; }
  bool isConnected() const { return m_connected.value() == kConnected; }

 private:
  void onConnect(bool sessionPresent);

  Options m_opts;
  TaskScheduler m_connect_scheduler;
  ConfigInterface* m_config = nullptr;
  WifiManager* m_wifi_manager = nullptr;
#ifndef NATIVE
  AsyncMqttClient m_mqttClient;
#endif
  SingleDependency m_dependency;

  VariableGroup m_vg;
  Variable<String> m_host_addr;
  // Variable<uint16_t> m_port;
  Variable<String> m_auth_user;
  Variable<String> m_auth_password;
  EnumStrVariable<Mode> m_mode;
  EnumStrVariable<ConnectionStatus> m_connected;

  String m_will_topic;

  std::vector<std::function<void()>> m_connectCallbacks;
  std::vector<std::function<void()>> m_disconnectCallbacks;

  struct MqttMsgCallback {
    String topic;
    MqttMsgCallbackFn callback_fn;
  };
  std::vector<MqttMsgCallback> m_mqtt_callbacks;
};

}  // namespace og3
