// Copyright (c) 2025 Chris Lee and contibuters.
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
// Absolute humidity in g/m³, mg/m³.
extern const char* kAbsolute_humidity;
// Apparent power in mVA, VA or kVA.
extern const char* kApparent_power;
// Air Quality Index (unitless).
extern const char* kAqi;
// Area in m², cm², km², mm², in², ft², yd², mi², ac, ha
extern const char* kArea;
// Atmospheric pressure in cbar, bar, hPa, mmHg, inHg, kPa, mbar, Pa or psi
extern const char* kAtmospheric_pressure;
// Percentage of battery that is left in %
extern const char* kBattery;
// Blood glucose concentration in mg/dL, mmol/L
extern const char* kBlood_glucose_concentration;
// Carbon Dioxide (CO₂) concentration in ppm
extern const char* kCarbon_dioxide;
// Carbon Monoxide (CO) concentration in ppm, µg/m³, mg/m³
extern const char* kCarbon_monoxide;
// Current in A, mA
extern const char* kCurrent;
// Data rate in bit/s, kbit/s, Mbit/s, Gbit/s, B/s, kB/s, MB/s, GB/s, KiB/s, MiB/s or GiB/s
extern const char* kDataRate;
// Data size in bit, kbit, Mbit, Gbit, B, kB, MB, GB, TB, PB, EB, ZB, YB, KiB, MiB, GiB, TiB, PiB,
// EiB, ZiB or YiB
extern const char* kDataSize;
// Date string (ISO 8601)
extern const char* kDate;
// Generic distance in km, m, cm, mm, mi, nmi, yd, or in
extern const char* kDistance;
// Duration in d, h, min, s, ms, or µs
extern const char* kDuration;
// Energy in J, kJ, MJ, GJ, mWh, Wh, kWh, MWh, GWh, TWh, cal, kcal, Mcal, or Gcal
extern const char* kEnergy;
// Energy per distance in kWh/100km, Wh/km, mi/kWh, or km/kWh.
extern const char* kEnergyDistance;
// Stored energy in J, kJ, MJ, GJ, mWh, Wh, kWh, MWh, GWh, TWh, cal, kcal, Mcal, or Gcal
extern const char* kEnergyStorage;
// Has a limited set of (non-numeric) states
extern const char* kEnum;
// Frequency in Hz, kHz, MHz, or GHz
extern const char* kFrequency;
// Gas volume in L, m³, ft³, CCF, or MCF
extern const char* kGas;
// Percentage of humidity in the air in %
extern const char* kHumidity;
// The current light level in lx
extern const char* kIlluminance;
// Irradiance in W/m² or BTU/(h⋅ft²)
extern const char* kIrradiance;
// Percentage of water in a substance in %
extern const char* kMoisture;
// The monetary value (ISO 4217)
extern const char* kMonetary;
// Concentration of Nitrogen Dioxide in µg/m³
extern const char* kNitrogenDioxide;
// Concentration of Nitrogen Monoxide in µg/m³
extern const char* kNitrogenMonoxide;
// Concentration of Nitrous Oxide in µg/m³
extern const char* kNitrousOxide;
// Concentration of Ozone in µg/m³
extern const char* kOzone;
// Potential hydrogen (pH) value of a water solution
extern const char* kPh;
// Concentration of particulate matter less than 1 micrometer in µg/m³
extern const char* kPm1;
// Concentration of particulate matter less than 2.5 micrometers in µg/m³
extern const char* kPm25;
// Concentration of particulate matter less than 4 micrometers in µg/m³
extern const char* kPm4;
// Concentration of particulate matter less than 10 micrometers in µg/m³
extern const char* kPm10;
// Power factor (unitless), unit may be None or %
extern const char* kPowerFactor;
// Power in mW, W, kW, MW, GW or TW
extern const char* kPower;
// Accumulated precipitation in cm, in or mm
extern const char* kPrecipitation;
// Precipitation intensity in in/d, in/h, mm/d or mm/h
extern const char* kPrecipitationIntensity;
// Pressure in mPa, Pa, hPa, kPa, bar, cbar, mbar, mmHg, inHg, inH₂O or psi
extern const char* kPressure;
// Reactive energy in varh or kvarh
extern const char* kReactiveEnergy;
// Reactive power in mvar, var, or kvar
extern const char* kReactivePower;
// Signal strength in dB or dBm
extern const char* kSignalStrength;
// Sound pressure in dB or dBA
extern const char* kSoundPressure;
// Generic speed in ft/s, in/d, in/h, in/s, km/h, kn, m/s, mph, mm/d, or mm/s
extern const char* kSpeed;
// Concentration of sulphur dioxide in µg/m³
extern const char* kSulphurDioxide;
// Temperature in °C, °F or K
extern const char* kTemperature;
// Temperature difference between two measurements in °C, °F, or K
extern const char* kTemperatureDelta;
// Datetime object or timestamp string (ISO 8601)
extern const char* kTimestamp;
// Concentration of volatile organic compounds in µg/m³ or mg/m³
extern const char* kVolatileOrganicCompounds;
// Ratio of volatile organic compounds in ppm or ppb
extern const char* kVolatileOrganicCompoundsParts;
// Voltage in V, mV, µV, kV, MV
extern const char* kVoltage;
// Generic volume in L, mL, gal, fl. oz., m³, ft³, CCF, or MCF
extern const char* kVolume;
// Volume flow rate in m³/h, m³/min, m³/s, ft³/min, L/h, L/min, L/s, gal/h, gal/min, or mL/s
extern const char* kVolumeFlowRate;
// Generic stored volume in L, mL, gal, fl. oz., m³, ft³, CCF, or MCF
extern const char* kVolumeStorage;
// Water consumption in L, gal, m³, ft³, CCF, or MCF
extern const char* kWater;
// Generic mass in kg, g, mg, µg, oz, lb, or st
extern const char* kWeight;
// Wind direction in °
extern const char* kWindDirection;
// Wind speed in Beaufort, ft/s, km/h, kn, m/s, or mph
extern const char* kWindSpeed;
}  // namespace sensor
namespace binary_sensor {
// on means charging, off means not charging
extern const char* kBatteryCharging;
// on means carbon monoxide detected, off no carbon monoxide (clear)
extern const char* kCarbonMonoxide;
// on means cold, off means normal
extern const char* kCold;
// on means connected, off means disconnected
extern const char* kConnectivity;
// on means open, off means closed
extern const char* kDoor;
// on means open, off means closed
extern const char* kGarageDoor;
// on means gas detected, off means no gas (clear)= "garage_door";
extern const char* kGas;
// on means hot, off means normal
extern const char* kHeat;
// on means light detected, off means no light
extern const char* kLight;
// on means open (unlocked), off means closed (locked)";
extern const char* kLock;
// on means moisture detected (wet), off means no moisture (dry)
extern const char* kMoisture;
// on means motion detected, off means no motion (clear)re";
extern const char* kMotion;
// on means moving, off means not moving (stopped)
extern const char* kMoving;
// on means occupied (detected), off means not occupied (clear)
extern const char* kOccupancy;
// on means open, off means closed
extern const char* kOpening;
// on means device is plugged in, off means device is unplugged
extern const char* kPlug;
// on means power detected, off means no power
extern const char* kPower;
// on means home, off means away
extern const char* kPresence;
//  on means problem detected, off means no problem (OK)
extern const char* kProblem;
// on means running, off means not running
extern const char* kRunning;
// on means unsafe, off means safe
extern const char* kSafety;
// on means smoke detected, off means no smoke (clear)
extern const char* kSmoke;
// on means sound detected, off means no sound (clear)
extern const char* kSound;
// on means tampering detected, off means no tampering (clear)
extern const char* kTamper;
// on means update available, off means up-to-date
extern const char* kUpdate;
// on means vibration detected, off means no vibration (clear)
extern const char* kVibration;
// on means open, off means closed
extern const char* kWindow;
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

  struct Entry {
    using ValueTemplateFn = std::function<String(const Entry&)>;
    Entry(const VariableBase& var_, const char* device_type_, const char* device_class_,
          const ValueTemplateFn& value_template = nullptr);
    Entry(const FloatVariableBase& var_, const char* device_type_, const char* device_class_);
    Entry(const BoolVariable& var_, const char* device_class_);
    Entry(const EnumStrVariableBase& var, const char* device_type);

    const VariableBase& var;
    const char* device_type;
    const char* device_class;
    const char* subject_topic = nullptr;
    const char* device_name = nullptr;
    const char* device_id = nullptr;     // HADiscovery device_id is used if not set.
    const char* manufacturer = nullptr;  // HADiscovery manufacturer is used if not set.
    const char* software = nullptr;      // HADiscovery software is used if not set.
    const char* model = nullptr;         // HADiscovery model is used if not set.
    const char* icon = nullptr;
    const char* state_class = nullptr;
    const char* via_device = nullptr;
    // Optionally overwrite var.name in discovery path: homeassistant/{device}/{entry_name|var.name}
    const char* entry_name = nullptr;
    // Command subject, if applicable
    const char* command = nullptr;
    // Callback to be registered for 'command', if set.
    MqttManager::MqttMsgCallbackFn command_callback;
    ValueTemplateFn value_template_fn;
    const unsigned num_options = 0;
    const char** options = nullptr;
  };
  bool addEntry(JsonDocument* json, const Entry& entry);
  bool addEntry(JsonDocument* json, const VariableBase& var, const char* device_type,
                const char* device_class, const Entry::ValueTemplateFn& value_template,
                const char* subject_topic, const char* device_name);
  bool addMeas(JsonDocument* json, const VariableBase& var, const char* device_type,
               const char* device_class, const char* subject_topic = nullptr,
               const char* device_name = nullptr);
  bool addMeas(JsonDocument* json, const FloatVariableBase& var, const char* device_type,
               const char* device_class, const char* subject_topic = nullptr,
               const char* device_name = nullptr);

  bool addEnum(JsonDocument* json, const EnumStrVariableBase& var, const char* device_type,
               const char* subject_topic = nullptr, const char* device_name = nullptr);
  bool addBinarySensor(JsonDocument* json, const VariableBase& var, const char* device_class,
                       const char* subject_topic = nullptr, const char* device_name = nullptr);
  bool addMotionSensor(JsonDocument* json, const VariableBase& var,
                       const char* subject_topic = nullptr, const char* device_name = nullptr);

  void addRoot(JsonDocument* json, const Entry& entry);

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

  bool m_config_sent = false;
  char m_device_id[32];
  std::vector<DiscoveryCallbackFn> m_discovery_callbacks;
  std::size_t m_idx_mqtt_connect_next_discovery_calback = 0;
};

}  // namespace og3
