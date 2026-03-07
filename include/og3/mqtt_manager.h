// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>

#include <cstdint>

#ifndef NATIVE
#if defined(ESP32)
#include <PsychicMqttClient.h>
#elif defined(ESP8266)
#include <AsyncMqttClient.h>
#endif
#endif

#include <functional>

#include "og3/logger.h"
#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/variable.h"
#include "og3/wifi.h"
#include "og3/wifi_manager.h"

namespace og3 {

#if defined(ESP32)
using MqttClient = PsychicMqttClient;  ///< @brief Underlying MQTT client for ESP32.
#elif defined(ESP8266)
using MqttClient = AsyncMqttClient;  ///< @brief Underlying MQTT client for ESP8266.
#else
class MqttClient;  ///< @brief Placeholder for non-ESP platforms.
#endif

/**
 * @brief Manages MQTT connectivity and message publishing/subscription.
 *
 * MqttManager handles connection establishment, automatic reconnection via WiFiManager,
 * and provides a high-level API for sending VariableGroup updates and subscribing to topics.
 * It also handles Home Assistant-style "will" messages for online/offline status.
 */
class MqttManager : public Module {
 public:
  static const char kName[];       ///< @brief "mqtt"
  static const char kConfigUrl[];  ///< @brief "/mqtt"

  /** @return Pointer to the MqttManager module instance. */
  static MqttManager* get(const NameToModule& n2m) { return GetModule<MqttManager>(n2m, kName); }

  /** @brief Supported MQTT integration modes. */
  enum class Mode {
    kHomeAssistant,  ///< Automatic discovery and state topics for HA.
    kAdafruitIO,     ///< Compatibility with Adafruit IO feed structure.
  };

  /** @brief Configuration options for the MQTT manager. */
  struct Options {
    Options() {}

    /** @brief Sets the application domain (prefix for topics). */
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

  /** @brief Constructs an MqttManager. */
  MqttManager(const Options& opts, Tasks* tasks);

  /** @brief Manually triggers a connection attempt. */
  void connect();
  /** @brief Manually disconnects from the MQTT server. */
  void disconnect();

  /** @brief Registers a callback for successful connection. */
  void addConnectCallback(const std::function<void()>& callback) {
    m_connectCallbacks.push_back(callback);
  }
  /** @brief Registers a callback for disconnection. */
  void addDisconnectCallback(const std::function<void()>& callback) {
    m_disconnectCallbacks.push_back(callback);
  }

#ifndef NATIVE
  /** @return Reference to the underlying hardware-specific MQTT client. */
  MqttClient& client() { return m_mqttClient; }
  /** @return Constant reference to the underlying MQTT client. */
  const MqttClient& client() const { return m_mqttClient; }
#endif

  /** @return true if the client is currently connected to the server. */
  bool connected() const {
#ifndef NATIVE
#if defined(ESP32)
    return const_cast<MqttClient&>(m_mqttClient).connected();
#else
    return m_mqttClient.connected();
#endif
#else
    return true;
#endif
  }

  /** @brief Publishes a raw string message to a specific topic. */
  void mqttSend(const char topic[], const char content[]);

  /**
   * @brief Publishes all variables in a group to their respective topics.
   * @param variables The VariableGroup to publish.
   * @param flags Filter flags for variable selection.
   * @return true if publishing was successful.
   */
  bool mqttSend(const VariableGroup& variables, unsigned flags = VariableBase::kNoPublish);

  /** @brief Callback type for received MQTT messages (topic, payload, len). */
  using MqttMsgCallbackFn = std::function<void(const char*, const char*, size_t)>;

  /** @brief Subscribes to an MQTT topic with a callback. */
  void subscribe(const String& topic, const MqttMsgCallbackFn& fn);

  /** @return The root topic for this board. */
  String boardTopic(const char* device_name = nullptr) const;
  /** @return A topic for a specific attribute name. */
  String topic(const char* name, const char* device_name = nullptr) const;
  /** @return The topic used for the LWT (Last Will and Testament). */
  String willTopic(const char* device_name = nullptr) const;

  /** @return The board name from WiFiManager. */
  const char* board() const {
    return m_wifi_manager ? m_wifi_manager->board().c_str() : "og3board";
  }
  /** @return The configured MQTT server address. */
  const String& host() const { return m_host_addr.value(); }
  /** @return The configured MQTT port. */
  uint16_t port() const { return m_opts.port; }
  /** @return The configured MQTT username. */
  const String& auth_user() const { return m_auth_user.value(); }
  /** @return The configured MQTT password. */
  const String& auth_password() const { return m_auth_password.value(); }
  /** @return true if MQTT is enabled in settings. */
  bool isEnabled() const { return m_enabled.value(); }
  /** @return The current MQTT integration mode. */
  Mode mode() const { return m_mode.value(); }

  /** @return Reference to the internal variables for MQTT configuration. */
  const VariableGroup& variables() const { return m_vg; }
  /** @return Reference to the internal variables for MQTT configuration. */
  VariableGroup& variables() { return m_vg; }

  /** @brief MQTT connection status enumeration. */
  enum ConnectionStatus { kNotConnected, kConnected };
  /** @return Variable tracking the connection status. */
  const EnumStrVariable<ConnectionStatus>& connectionStatusVariable() const { return m_connected; }
  /** @return true if connected. */
  bool isConnected() const { return m_connected.value() == kConnected; }

 private:
  void onConnect(bool sessionPresent);

  Options m_opts;
  TaskScheduler m_connect_scheduler;
  ConfigInterface* m_config = nullptr;
  WifiManager* m_wifi_manager = nullptr;
#ifndef NATIVE
  MqttClient m_mqttClient;
#endif

  VariableGroup m_vg;
  BoolVariable m_enabled;
  Variable<String> m_host_addr;
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
