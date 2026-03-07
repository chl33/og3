// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>

#include <functional>

#include "og3/module.h"
#include "og3/mqtt_manager.h"
#include "og3/tasks.h"

namespace og3 {
class Tasks;
}

namespace og3 {

/** @brief Home Assistant device types. */
namespace ha::device_type {
extern const char* kBinarySensor;  ///< "binary_sensor"
extern const char* kClimate;       ///< "climate"
extern const char* kCover;         ///< "cover"
extern const char* kSensor;        ///< "sensor"
extern const char* kSwitch;        ///< "switch"
}  // namespace ha::device_type

/** @brief Home Assistant device classes for various entity types. */
namespace ha::device_class {

/** @brief Device classes for 'cover' entities. */
namespace cover {
extern const char* kAwning;
extern const char* kBlind;
extern const char* kCurtain;
extern const char* kDamper;
extern const char* kDoor;
extern const char* kGarage;
extern const char* kGate;
extern const char* kShade;
extern const char* kShutter;
extern const char* kWindow;
}  // namespace cover

/** @brief Device classes for 'sensor' entities. */
namespace sensor {
extern const char* kAbsolute_humidity;
extern const char* kApparent_power;
extern const char* kAqi;
extern const char* kArea;
extern const char* kAtmosphericPressure;
extern const char* kBattery;
extern const char* kBlood_glucose_concentration;
extern const char* kCarbon_dioxide;
extern const char* kCarbon_monoxide;
extern const char* kCurrent;
extern const char* kDataRate;
extern const char* kDataSize;
extern const char* kDate;
extern const char* kDistance;
extern const char* kDuration;
extern const char* kEnergy;
extern const char* kEnergyDistance;
extern const char* kEnergyStorage;
extern const char* kEnum;
extern const char* kFrequency;
extern const char* kGas;
extern const char* kHumidity;
extern const char* kIlluminance;
extern const char* kIrradiance;
extern const char* kMoisture;
extern const char* kMonetary;
extern const char* kNitrogenDioxide;
extern const char* kNitrogenMonoxide;
extern const char* kNitrousOxide;
extern const char* kOzone;
extern const char* kPh;
extern const char* kPm1;
extern const char* kPm25;
extern const char* kPm4;
extern const char* kPm10;
extern const char* kPowerFactor;
extern const char* kPower;
extern const char* kPrecipitation;
extern const char* kPrecipitationIntensity;
extern const char* kPressure;
extern const char* kReactiveEnergy;
extern const char* kReactivePower;
extern const char* kSignalStrength;
extern const char* kSoundPressure;
extern const char* kSpeed;
extern const char* kSulphurDioxide;
extern const char* kTemperature;
extern const char* kTemperatureDelta;
extern const char* kTimestamp;
extern const char* kVolatileOrganicCompounds;
extern const char* kVolatileOrganicCompoundsParts;
extern const char* kVoltage;
extern const char* kVolume;
extern const char* kVolumeFlowRate;
extern const char* kVolumeStorage;
extern const char* kWater;
extern const char* kWeight;
extern const char* kWindDirection;
extern const char* kWindSpeed;
}  // namespace sensor

/** @brief Device classes for 'binary_sensor' entities. */
namespace binary_sensor {
extern const char* kBatteryCharging;
extern const char* kCarbonMonoxide;
extern const char* kCold;
extern const char* kConnectivity;
extern const char* kDoor;
extern const char* kGarageDoor;
extern const char* kGas;
extern const char* kHeat;
extern const char* kLight;
extern const char* kLock;
extern const char* kMoisture;
extern const char* kMotion;
extern const char* kMoving;
extern const char* kOccupancy;
extern const char* kOpening;
extern const char* kPlug;
extern const char* kPower;
extern const char* kPresence;
extern const char* kProblem;
extern const char* kRunning;
extern const char* kSafety;
extern const char* kSmoke;
extern const char* kSound;
extern const char* kTamper;
extern const char* kUpdate;
extern const char* kVibration;
extern const char* kWindow;
}  // namespace binary_sensor
}  // namespace ha::device_class

class ConfigInterface;
class WifiManager;

/**
 * @brief Module for automating Home Assistant MQTT Discovery.
 *
 * HADiscovery registers Variable objects as entities in Home Assistant by publishing
 * configuration payloads to specific MQTT topics. It handles device metadata,
 * state reporting, and command callbacks.
 */
class HADiscovery : public Module {
 public:
  /** @brief Configuration for the device as seen in Home Assistant. */
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

  static const char* kName;  ///< @brief "ha_discovery"

  /** @brief Constructs HADiscovery module. */
  HADiscovery(const Options& opts, ModuleSystem* module_system);

  /** @brief Represents a single entity entry for HA Discovery. */
  struct Entry {
    /** @brief Callback type for generating custom value templates. */
    using ValueTemplateFn = std::function<String(const Entry&)>;

    /** @brief Constructs a general variable entry. */
    Entry(const VariableBase& var_, const char* device_type_, const char* device_class_,
          const ValueTemplateFn& value_template = nullptr);
    /** @brief Constructs a floating-point sensor entry. */
    Entry(const FloatVariableBase& var_, const char* device_type_, const char* device_class_);
    /** @brief Constructs a boolean sensor entry. */
    Entry(const BoolVariable& var_, const char* device_class_);
    /** @brief Constructs an enumeration entry. */
    Entry(const EnumStrVariableBase& var, const char* device_type);

    const VariableBase& var;
    const char* device_type;
    const char* device_class;
    const char* subject_topic = nullptr;  ///< MQTT topic for state updates.
    const char* device_name = nullptr;    ///< Override device name.
    const char* device_id = nullptr;      ///< Override device unique ID.
    const char* manufacturer = nullptr;
    const char* software = nullptr;
    const char* model = nullptr;
    const char* icon = nullptr;
    const char* state_class = nullptr;
    const char* via_device = nullptr;
    const char* entry_name = nullptr;                 ///< Override entity name.
    const char* command = nullptr;                    ///< MQTT topic for receiving commands.
    MqttManager::MqttMsgCallbackFn command_callback;  ///< Callback for commands.
    ValueTemplateFn value_template_fn;
    const unsigned num_options = 0;
    const char** options = nullptr;
  };

  /** @brief Registers an entity by adding its config to a JSON payload. */
  bool addEntry(JsonDocument* json, const Entry& entry);

  /** @brief Convenience method to register a generic entity. */
  bool addEntry(JsonDocument* json, const VariableBase& var, const char* device_type,
                const char* device_class, const Entry::ValueTemplateFn& value_template,
                const char* subject_topic, const char* device_name);

  /** @brief Registers a numeric measurement sensor. */
  bool addMeas(JsonDocument* json, const VariableBase& var, const char* device_type,
               const char* device_class, const char* subject_topic = nullptr,
               const char* device_name = nullptr);

  /** @brief Registers a floating-point measurement sensor. */
  bool addMeas(JsonDocument* json, const FloatVariableBase& var, const char* device_type,
               const char* device_class, const char* subject_topic = nullptr,
               const char* device_name = nullptr);

  /** @brief Registers an enumeration sensor. */
  bool addEnum(JsonDocument* json, const EnumStrVariableBase& var, const char* device_type,
               const char* subject_topic = nullptr, const char* device_name = nullptr);

  /** @brief Registers a binary sensor. */
  bool addBinarySensor(JsonDocument* json, const VariableBase& var, const char* device_class,
                       const char* subject_topic = nullptr, const char* device_name = nullptr);

  /** @brief Registers a motion binary sensor. */
  bool addMotionSensor(JsonDocument* json, const VariableBase& var,
                       const char* subject_topic = nullptr, const char* device_name = nullptr);

  /** @brief Internal helper to populate common device root information in JSON. */
  void addRoot(JsonDocument* json, const Entry& entry);

  /** @brief Publishes a discovery configuration message. */
  bool mqttSendConfig(const char* name, const char* device_class, JsonDocument* json);

  /** @brief Subscribes to a command topic. */
  void mqttSubscribe(const char* name, const MqttManager::MqttMsgCallbackFn& fn);

  /** @brief Callback type for performing discovery after MQTT connection. */
  using DiscoveryCallbackFn = std::function<bool(HADiscovery*, JsonDocument*)>;

  /** @brief Adds a callback to be executed whenever the module needs to re-send discovery. */
  void addDiscoveryCallback(const DiscoveryCallbackFn& callback_fn) {
    m_discovery_callbacks.push_back(callback_fn);
  }

  /** @return The unique device ID used for discovery. */
  const char* deviceId() const { return m_device_id; }

  /** @return Pointer to the HADiscovery module instance. */
  static HADiscovery* get(const NameToModule& n2m) { return GetModule<HADiscovery>(n2m, kName); }

  /** @return Reference to the options used. */
  const Options& options() const { return m_options; }

  /** @return true if MQTT is enabled and in Home Assistant mode. */
  bool enabled() const {
    return m_mqtt_manager && m_mqtt_manager->mode() == MqttManager::Mode::kHomeAssistant;
  }

  /** @return The base MQTT topic for this board. */
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

  bool m_config_sent = false;
  char m_device_id[32];
  std::vector<DiscoveryCallbackFn> m_discovery_callbacks;
  std::size_t m_idx_mqtt_connect_next_discovery_calback = 0;
};

}  // namespace og3
