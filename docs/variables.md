## Variables

A [`og3::Variable`](../include/og3/variable.h) is a named container for a value of a given type.  Variable types include integer, float, double, string, and enumerations.  A variable must be a member of a `VariableGroup`, a collection of variables together which can be loaded from a configuration file, edited via a web form, or published via MQTT.  Each variable is declared with a set of flags which specify whether it should be settable via web forms, whether it should be saved to configuration files in flash, whether it should be published via MQTT, and whether it should be included in HTML tables listing values.

An example of declaring variable groups for configuring and monitoring a plant watering system is shown below.
```C++
  // A variable group for configuring the plant-watering process.
  VariableGroup cfg_vg("watering_config");
  // A variable group for monitoring the plant-watering process.
  VariableGroup vg("watering");

  // Flags for configuration variables: settable via web forms, and saved to flash.
  constexpr unsigned kCfgFlags = (VariableBase::Flags::kConfig
                                  | VariableBase::Flags::kSettable);
  // Flags for monitored variables: default is published, not configed, not settable.
  constexpr unsigned kVarFlags = 0;

  // Configuration variable: whether the watering process is enabled.
  // The default value is false.
  BoolVariable watering_enabled("watering_enabled", false, "watering enabled",
                                kCfgFlags, &vg);
  // Configuration variable: the soil moisture level above which watering stops
  // Values should be printed with on digit after the decimal point.
  // The default value is 80, the unit is "%".
  FloatVariable max_moisture("max_moisture_target", 80.0f, units::kPercentage,
                             "Max moisture", kCfgFlags, 1, &cfg_vg);
  // Configuration variable: the soil moisture level below which when watering starts
  FloatVariable min_moisture("min_moisture_target", 60.0f, units::kPercentage,
                             "Min moisture", kCfgFlags, 1, &cfg_vg);
  // Monitored variable variable: the meaured soil moisture level.
  // The default value is 0.
  FloatVariable moisture("soil_moisture", 0.0, "%", "soil moisture", kVarFlags, 1, &vg);
```
