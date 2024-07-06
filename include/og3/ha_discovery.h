// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>

#include "og3/module.h"
#include "og3/mqtt_manager.h"
#include "og3/tasks.h"

namespace og3 {
class Tasks;
}

namespace og3 {
// Home assistant device types
namespace ha::device_type {
extern const char* kBinarySensor;
extern const char* kClimate;
extern const char* kCover;
extern const char* kSensor;
extern const char* kSwitch;
}  // namespace ha::device_type

namespace ha::device_class {
// Sensors https://www.home-assistant.io/integrations/sensor
namespace sensor {
extern const char* kAtmosphericPressure;
extern const char* kDistance;
extern const char* kDuration;
extern const char* kHumidity;
extern const char* kMoisture;
extern const char* kTemperature;
extern const char* kSignalStrength;
extern const char* kVoltage;
}  // namespace sensor
namespace binary_sensor {
extern const char* kMoisture;
extern const char* kMotion;
extern const char* kPower;
extern const char* kLight;
}  // namespace binary_sensor
}  // namespace ha::device_class

class ConfigInterface;
class WiFiManager;

// HADiscovery is a module which assists with registering the value of Variables as
//  Home Assistant Entities.
// It does this by providing an API for sending particular MQTT messsages to declare
//  these entities.
class HADiscovery : public Module {
 public:
  struct Options {
    Options(const char* manufacturer_, const char* model_, const char* software_,
            const char* device_name_ = nullptr)
        : manufacturer(manufacturer_),
          model(model_),
          software(software_),
          device_name(device_name_) {}
    const char* manufacturer;
    const char* model;
    const char* software;
    const char* device_name;
  };

  static const char* kName;

  HADiscovery(const Options& opts, ModuleSystem* module_system);

  void addRoot(JsonDocument* json, const char* device_name);
  bool addEntry(JsonDocument* json, const VariableBase& var, const char* device_type,
                const char* device_class, const char* value_template, const char* subject_topic,
                const char* device_name);
  bool addMeas(JsonDocument* json, const VariableBase& var, const char* device_type,
               const char* device_class, const char* subject_topic = nullptr,
               const char* device_name = nullptr);
  bool addMeas(JsonDocument* json, const FloatVariableBase& var, const char* device_type,
               const char* device_class, const char* subject_topic = nullptr,
               const char* device_name = nullptr);
  bool addBinarySensor(JsonDocument* json, const VariableBase& var, const char* device_class,
                       const char* subject_topic = nullptr, const char* device_name = nullptr);
  bool addMotionSensor(JsonDocument* json, const VariableBase& var,
                       const char* subject_topic = nullptr, const char* device_name = nullptr);

#if 0
  void addTempC(const char* name, int decimals, JsonDocument* json);
  void addTempF(const char* name, int decimals, JsonDocument* json);
  void addHumidity(const char* name, int decimals, JsonDocument* json);
  void addCountsMeas(const char* name, const char* device_class, JsonDocument* json);
  void addCover(const char* name, const char* command_topic, const char* device_class,
                JsonDocument* json);
  void addCommand(const char* name, const char* cmd_atom, JsonDocument* json);
  void addIcon(const char* icon_name, JsonDocument* json);

#endif
  bool mqttSendConfig(const char* name, const char* device_class, JsonDocument* json);
  void mqttSubscribe(const char* name, const MqttManager::MqttMsgCallbackFn& fn);

  using DiscoveryCallbackFn = std::function<bool(HADiscovery*, JsonDocument*)>;
  void addDiscoveryCallback(const DiscoveryCallbackFn& callback_fn) {
    m_discovery_callbacks.push_back(callback_fn);
  }

  // This is available after depend-system init.
  const char* deviceId() const { return m_device_id; }

  static HADiscovery* get(const NameToModule& n2m) { return GetModule<HADiscovery>(n2m, kName); }

  const Options& options() const { return m_options; }

  bool enabled() const {
    return m_mqtt_manager && m_mqtt_manager->mode() == MqttManager::Mode::kHomeAssistant;
  }
  const String& boardTopic();

 private:
  void onMqttConnect();

  const Options m_options;
  String m_mac_address;
  String m_device_name;
  String m_board_topic_;
  Tasks* m_tasks = nullptr;
  ConfigInterface* m_config = nullptr;
  WifiManager* m_wifi_manager = nullptr;
  MqttManager* m_mqtt_manager = nullptr;
  SingleDependency m_dependency;

  bool m_config_sent = false;
  char m_device_id[32];
  std::vector<DiscoveryCallbackFn> m_discovery_callbacks;
  std::size_t m_idx_mqtt_connect_next_discovery_calback = 0;
};

}  // namespace og3
