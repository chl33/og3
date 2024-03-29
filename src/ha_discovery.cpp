// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/ha_discovery.h"

#include "og3/config_interface.h"
#include "og3/logger.h"
#include "og3/mqtt_manager.h"
#include "og3/tasks.h"
#include "og3/wifi.h"
#include "og3/wifi_manager.h"

namespace og3 {
namespace {
bool strOk(const char* str) { return str && str[0]; }
}  // namespace

namespace ha::device_type {
const char* kBinarySensor = "binary_sensor";
const char* kClimate = "climate";
const char* kCover = "cover";
const char* kSensor = "sensor";
const char* kSwitch = "switch";
}  // namespace ha::device_type

namespace ha::device_class {
// Sensors https://www.home-assistant.io/integrations/sensor
namespace sensor {
const char* kAtmosphericPressure = "atmospheric_pressure";
const char* kDuration = "duration";
const char* kHumidity = "humidity";
const char* kMoisture = "moisture";
const char* kTemperature = "temperature";
const char* kSignalStrength = "signal_strength";
const char* kVoltage = "voltage";
}  // namespace sensor

namespace binary_sensor {
const char* kMoisture = "moisture";
const char* kMotion = "motion";
const char* kPower = "power";
const char* kLight = "light";
}  // namespace binary_sensor
}  // namespace ha::device_class

const char* HADiscovery::kName = "ha_discovery";

HADiscovery::HADiscovery(const Options& opts, ModuleSystem* module_system)
    : Module(kName, module_system), m_options(opts), m_dependency(MqttManager::kName) {
  setDependencies(&m_dependency);
  if (!opts.manufacturer) {
    log()->logf("%s: manufacturer not set", kName);
    delay(10000);
    return;
  }
  if (!opts.model) {
    log()->logf("%s: model not set", kName);
    delay(10000);
    return;
  }
  if (!opts.software) {
    log()->logf("%s: software not set", kName);
    delay(10000);
    return;
  }

#ifndef NATIVE
  m_mac_address = WiFi.macAddress();
#endif

  add_link_fn([this](NameToModule& name_to_module) -> bool {
    m_config = ConfigInterface::get(name_to_module);
    m_tasks = Tasks::get(name_to_module);
    m_wifi_manager = WifiManager::get(name_to_module);
    m_mqtt_manager = MqttManager::get(name_to_module);
    return m_config && m_wifi_manager && m_mqtt_manager;
  });
  add_init_fn([this]() {
    if (m_mqtt_manager) {
      m_device_name = m_options.device_name ? m_options.device_name : m_wifi_manager->board();
      m_mqtt_manager->addConnectCallback([this] { onMqttConnect(); });
    }
  });

  size_t i = 0;
  for (const char c : m_mac_address) {
    if (c == ':' || i >= sizeof(m_device_id) - 1) {
      continue;
    }
    m_device_id[i] = c;
    i += 1;
  }
  snprintf(m_device_id + i, sizeof(m_device_id) - i, "_%s", m_options.model);
}

const String& HADiscovery::boardTopic() {
  const bool empty = m_board_topic_.length() == 0;
  if (empty) {
    m_board_topic_ = m_mqtt_manager->boardTopic();
  }
  return m_board_topic_;
}

void HADiscovery::addRoot(JsonDocument* json, const char* device_name) {
  if (!enabled()) {
    return;
  }
  auto& js = *json;
  // Root path
  js["~"] = boardTopic().c_str();
  // Availability topic
  js["avty_t"] = "~/connection";
  js["pl_avail"] = "online";
  js["pl_not_avail"] = "offline";

  JsonObject device = js["device"].to<JsonObject>();
  device["name"] = device_name ? device_name : m_device_name.c_str();
  device["ids"] = m_device_id;
  device["mf"] = m_options.manufacturer;
  device["mdl"] = m_options.model;
  device["sw"] = m_options.software;
#ifndef NATIVE
  device["cu"] = "http://" + WiFi.localIP().toString() + "/";
#endif
}

bool HADiscovery::addEntry(JsonDocument* json, const VariableBase& var, const char* device_type,
                           const char* device_class, const char* value_template,
                           const char* subject_topic, const char* device_name) {
  if (!enabled()) {
    return true;
  }
  auto& js = *json;
  js.clear();
  addRoot(json, device_name);

  char value[128];
  if (strOk(var.units())) {
    js["unit_of_meas"] = var.units();
  }
  {
    const char* subject = subject_topic ? subject_topic : "~";
    if (!var.group()) {
      js["stat_t"] = subject;
    } else {
      snprintf(value, sizeof(value), "%s/%s", subject, var.group()->name());
      js["stat_t"] = value;
    }
  }
  js["val_tpl"] = value_template;
  if (device_class) {
    js["dev_cla"] = device_class;
  }
  js["name"] = var.name();
  snprintf(value, sizeof(value), "%s_%s", m_device_id, var.name());
  js["uniq_id"] = value;

  return mqttSendConfig(var.name(), device_type, json);
}

bool HADiscovery::addMeas(JsonDocument* json, const VariableBase& var, const char* device_type,
                          const char* device_class, const char* subject_topic,
                          const char* device_name) {
  char value_template[128];
  snprintf(value_template, sizeof(value_template), "{{value_json.%s}}", var.name());
  return addEntry(json, var, device_type, device_class, value_template, subject_topic, device_name);
}

bool HADiscovery::addMeas(JsonDocument* json, const FloatVariableBase& var, const char* device_type,
                          const char* device_class, const char* subject_topic,
                          const char* device_name) {
  char value_template[128];
  snprintf(value_template, sizeof(value_template), "{{value_json.%s|float|round(%d)}}", var.name(),
           var.decimals());
  return addEntry(json, var, device_type, device_class, value_template, subject_topic, device_name);
}

bool HADiscovery::addBinarySensor(JsonDocument* json, const VariableBase& var,
                                  const char* device_class, const char* subject_topic,
                                  const char* device_name) {
  char value_template[128];
  snprintf(value_template, sizeof(value_template), "{{value_json.%s}}", var.name());
  return addEntry(json, var, ha::device_type::kBinarySensor, device_class, value_template,
                  subject_topic, device_name);
}

bool HADiscovery::addMotionSensor(JsonDocument* json, const VariableBase& var,
                                  const char* subject_topic, const char* device_name) {
  return addBinarySensor(json, var, ha::device_class::binary_sensor::kMotion, subject_topic,
                         device_name);
}

#if 0
void HADiscovery::addCountsMeas(const char* name, const char* device_class, JsonDocument* json) {
  char value[128];
  // (*json)["unit_of_meas"] = "count";
  (*json)["stat_t"] = "~";
  snprintf(value, sizeof(value), "{{value_json.%s|int}}", name);
  (*json)["val_tpl"] = value;
  if (device_class) {
    (*json)["dev_cla"] = device_class;
  }
  snprintf(value, sizeof(value), "%s %s", m_device_name(), name);
  (*json)["name"] = value;
  snprintf(value, sizeof(value), "%s_%s", m_device_id, name);
  (*json)["uniq_id"] = value;
}

void HADiscovery::addCover(const char* name, const char* command_topic, const char* device_class,
                           JsonDocument* json) {
  char value[128];
  if (device_class) {
    (*json)["dev_cla"] = device_class;
  }
  (*json)["stat_t"] = "~";
  snprintf(value, sizeof(value), "%s %s", m_device_name(), name);
  (*json)["name"] = value;
  snprintf(value, sizeof(value), "{{value_json.%s}}", name);
  (*json)["val_tpl"] = value;
  snprintf(value, sizeof(value), "%s_%s", m_device_id, name);
  (*json)["uniq_id"] = value;
  if (command_topic) {
    (*json)["cmd_t"] = command_topic;
  }
}

void HADiscovery::addCommand(const char* name, const char* cmd_atom, JsonDocument* json) {
  char topic[128];
  snprintf(topic, sizeof(topic), "~/%s/set", name);
  (*json)[cmd_atom] = topic;
}

void HADiscovery::addIcon(const char* icon_name, JsonDocument* json) { (*json)["ic"] = icon_name; }
#endif

bool HADiscovery::mqttSendConfig(const char* name, const char* device_type, JsonDocument* json) {
  if (!enabled()) {
    return true;
  }
#ifndef NATIVE
  String content;
  serializeJson(*json, content);
  char topic[256];
  snprintf(topic, sizeof(topic), "homeassistant/%s/%s/%s/config", device_type,
           m_device_name.c_str(), name);
  log()->debugf("sendConfig: '%s'\n %s", topic, content.c_str());
  const int packetIdPub = m_mqtt_manager->client().publish(topic, 1, true, content.c_str());
  if (packetIdPub == 0) {
    log()->logf("%ld Failed to publish config on topic %s.", millis(), topic);
    return false;
  }
  log()->logf("%ld Publishing config '%s' packetId: %d\n %s.", millis(), topic, packetIdPub,
              content.c_str());
#endif
  return true;
}

void HADiscovery::mqttSubscribe(const char* name, const MqttManager::MqttMsgCallbackFn& fn) {
  if (!enabled()) {
    return;
  }
  char topic[128];
  snprintf(topic, sizeof(topic), "%s", m_mqtt_manager->topic(name).c_str());
  m_mqtt_manager->subscribe(topic, fn);
}

void HADiscovery::onMqttConnect() {
  if (!enabled()) {
    return;
  }
  if (!m_wifi_manager) {
    return;
  }

  auto tryAgain = [this]() {
    // Queuing config to mqtt to send failed, so retry in a little while.
    log()->logf("%ld HA discovery onMqttConect failed at %zu/%zu: pausing.", millis(),
                m_idx_mqtt_connect_next_discovery_calback, m_discovery_callbacks.size());
    // Call back in 100msec.
    m_tasks->runIn(1000, std::bind(&HADiscovery::onMqttConnect, this));
  };

  JsonDocument m_json;
  while (m_idx_mqtt_connect_next_discovery_calback < m_discovery_callbacks.size()) {
    auto& callback = m_discovery_callbacks[m_idx_mqtt_connect_next_discovery_calback];
    if (!callback(this, &m_json)) {
      tryAgain();
      return;
    }
    m_idx_mqtt_connect_next_discovery_calback += 1;
  }
}

}  // namespace og3
