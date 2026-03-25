// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/mqtt_manager.h"

#include <ArduinoJson.h>

#include <functional>

#include "og3/config_interface.h"
#include "og3/constants.h"
#include "og3/html_table.h"
#include "og3/web_server.h"
#include "og3/wifi_manager.h"

namespace {
const char* s_str_modes[] = {"home-assistant", "adafruit.io."};
const char* s_str_connected[] = {"not connected", "connected"};
}  // namespace

namespace og3 {

const char MqttManager::kName[] = "mqtt";
const char MqttManager::kConfigUrl[] = "/mqtt/update";

String MqttManager::boardTopic(const char* device_name) const {
  log()->debugf("board(): %s", board() ? board() : "nullptr");
  const char* device = device_name ? device_name : board();
  return String(m_opts.app_domain ? m_opts.app_domain : "app") + "/" + device;
}

String MqttManager::topic(const char* name, const char* device_name) const {
  const char* device = device_name ? device_name : board();
  switch (mode()) {
    case Mode::kHomeAssistant:
      return String(m_opts.app_domain ? m_opts.app_domain : "app") + "/" + device + "/" + name;
    case Mode::kAdafruitIO:
      return m_auth_user.value() + "/feeds/" + device + "_" + name;
  }
  return "";
}

String MqttManager::willTopic(const char* device_name) const {
  return topic("connection", device_name);
}

MqttManager::MqttManager(const Options& opts, Tasks* tasks)
    : Module(kName, tasks->module_system()),
      m_opts(opts),
      m_connect_scheduler([this]() { connect(); }, tasks),
      m_vg(kName, nullptr, 4),
      m_enabled("enabled", true, "Enable MQTT",
                VariableBase::kConfig | VariableBase::kSettable | VariableBase::kNoPublish, m_vg),
      m_host_addr("hostAddr", opts.default_server, "", "MQTT server",
                  VariableBase::kConfig | VariableBase::kSettable, m_vg),
#if 0
      m_port("port", opts.port, "", "port",
                  VariableBase::kConfig | VariableBase::kSettable | VariableBase::kNoPublish, m_vg),
#endif
      m_auth_user("authUser", opts.default_user, "", "username",
                  VariableBase::kConfig | VariableBase::kSettable, m_vg),
      m_auth_password("authPassword", opts.default_password, "", "password",
                      VariableBase::kConfig | VariableBase::kSettable | VariableBase::kNoPublish |
                          VariableBase::kNoDisplay,
                      m_vg),
      m_mode("mode", opts.mode, "mode", Mode::kAdafruitIO, s_str_modes,
             VariableBase::kConfig | VariableBase::kSettable | VariableBase::kNoPublish, m_vg),
      m_connected("connection", kNotConnected, "connection", kConnected, s_str_connected,
                  VariableBase::kNoPublish, m_vg) {
  // Module callbacks
  require(ConfigInterface::kName, &m_config);
  require(WifiManager::kName, &m_wifi_manager);

  add_init_fn([this]() {
    if (m_config) {
      m_config->read_config(m_vg);
    }
    if (m_wifi_manager) {
      m_wifi_manager->addConnectCallback([this] { connect(); });
    }
  });

#ifndef NATIVE
  // Configure mqtt client callbacks.
#if defined(ESP32)
  m_mqttClient.onConnect([this](bool sessionPresent) { return onConnect(sessionPresent); });
  m_mqttClient.onDisconnect([this](int reason) {
    m_connected = kNotConnected;
    log()->logf("Disconnected from MQTT. Reason: %d.", reason);
    if (WiFi.isConnected()) {
      // Try again in 10 seconds.
      m_connect_scheduler.runIn(10 * kMsecInSec);
    }
  });
  m_mqttClient.onPublish(
      [this](int packetId) { log()->debugf("Publish acknowledged. packetId: %d", packetId); });
  m_mqttClient.onSubscribe(
      [this](int packetId) { log()->logf("Subscription acknowledged. packetId: %d", packetId); });
  m_mqttClient.onMessage([this](char* topic, char* payload, int len, int index, bool total) {
    for (const auto& it : m_mqtt_callbacks) {
      if (it.topic == topic) {
        it.callback_fn(topic, payload, static_cast<size_t>(len));
        return;
      }
    }
  });
#elif defined(ESP8266)
  m_mqttClient.onConnect([this](bool sessionPresent) { return onConnect(sessionPresent); });
  m_mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
    m_connected = kNotConnected;
    log()->logf("Disconnected from MQTT. Reason: %d.", (int)reason);
    if (WiFi.isConnected()) {
      m_connect_scheduler.runIn(10 * kMsecInSec);
    }
  });
  m_mqttClient.onPublish([this](uint16_t packetId) {
    log()->debugf("Publish acknowledged. packetId: %u", static_cast<unsigned>(packetId));
  });
  m_mqttClient.onSubscribe([this](uint16_t packetId, uint8_t qos) {
    log()->logf("Subscription acknowledged. packetId: %u", static_cast<unsigned>(packetId));
  });
  m_mqttClient.onMessage([this](char* topic, char* payload,
                                AsyncMqttClientMessageProperties properties, size_t len,
                                size_t index, size_t total) {
    for (const auto& it : m_mqtt_callbacks) {
      if (it.topic == topic) {
        it.callback_fn(topic, payload, len);
        return;
      }
    }
  });
#endif
#endif
}

void MqttManager::disconnect() {
#ifndef NATIVE
  if (connected()) {
    log()->log("Disconnecting from MQTT...");
    m_mqttClient.disconnect();
  }
#endif
}

void MqttManager::connect() {
#ifndef NATIVE
  if (!isEnabled()) {
    log()->log("Skipping MQTT connect: disabled.");
    return;
  }
  if (m_wifi_manager->apMode()) {
    log()->log("Skipping MQTT connect: in access-point mode.");
    return;
  }
  const auto& host = m_host_addr.value();
  const auto& user = m_auth_user.value();
  if (host.length() == 0 || user.length() == 0) {
    log()->log("Skipping MQTT connect: no hostname/user");
    return;
  }

#if defined(ESP32)
  // PsychicMqttClient uses a URI: mqtt://host:port
  String uri = "mqtt://" + host + ":" + String(m_opts.port);
  log()->logf("MQTT uri: '%s'", uri.c_str());
  m_mqttClient.setServer(uri.c_str());
#elif defined(ESP8266)
  log()->logf("MQTT h:'%s':%d", host.c_str(), (int)m_opts.port);
  m_mqttClient.setServer(host.c_str(), m_opts.port);
#endif

  const auto& password = m_auth_password.value();
  log()->logf("MQTT u: '%s'", user.c_str());
  m_mqttClient.setCredentials(user.c_str(), password.c_str());

  m_will_topic = willTopic();
  if (m_will_topic.length()) {
    log()->logf("willTopic: '%s'", m_will_topic.c_str());
    m_mqttClient.setWill(m_will_topic.c_str(), m_opts.will_qos, m_opts.will_retain, "offline");
  }
  log()->log("Connecting to MQTT...");
  m_mqttClient.connect();
#endif
}

void MqttManager::onConnect(bool sessionPresent) {
  m_connected = kConnected;
#ifndef NATIVE
  log()->logf("Connected to MQTT. Session: %s.", sessionPresent ? "true" : "false");

  const auto will_topic = willTopic();
  if (will_topic.length()) {
    log()->debugf("Printing will");
#if defined(ESP32)
    m_mqttClient.publish(will_topic.c_str(), m_opts.will_qos, m_opts.will_retain, "online");
#elif defined(ESP8266)
    m_mqttClient.publish(will_topic.c_str(), m_opts.will_qos, m_opts.will_retain, "online");
#endif
  }
  log()->debugf("mqtt connection callbacks: %d", static_cast<int>(m_connectCallbacks.size()));
  for (const auto& callback : m_connectCallbacks) {
    callback();
  }
#endif
}

void MqttManager::subscribe(const String& topic, const MqttMsgCallbackFn& fn) {
#ifndef NATIVE
  m_mqtt_callbacks.push_back({topic, fn});
  const int packetIdSub = m_mqttClient.subscribe(topic.c_str(), 2);
  log()->logf("Subscribing to '%s' (id=%d)", topic.c_str(), packetIdSub);
#endif
}

void MqttManager::mqttSend(const char topic[], const char content[]) {
  if (!connected()) {
    return;
  }
#ifndef NATIVE
  m_mqttClient.publish(topic, 1, true, content);
  log()->debugf("%ld Publishing on topic '%s' at QoS 1", millis(), topic);
#endif
}

bool MqttManager::mqttSend(const VariableGroup& variables, unsigned flags) {
  if (!connected()) {
    return false;
  }
  String mqttOutput;
  switch (mode()) {
    case Mode::kHomeAssistant: {
      variables.toJson(&mqttOutput, flags);
      break;
    }
    case Mode::kAdafruitIO:
      String values;
      variables.toJson(&values, flags);
      mqttOutput = String("value:") + values;
      break;
  }
  mqttSend(topic(variables.id()).c_str(), mqttOutput.c_str());
  return true;
}

}  // namespace og3
