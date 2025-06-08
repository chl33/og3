// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <og3/config_module.h>
#include <og3/mapped_analog_sensor.h>

namespace og3 {

class AdcVoltage : public ConfigModule {
  // voltage divider with 10k, 20k(mez) R.
 public:
  AdcVoltage(const char* name, WifiApp* app, uint8_t pin, const char* raw_desc, const char* desc,
             float out_max, VariableGroup& vg, VariableGroup& config_vg);

  float read() { return m_mapped_adc.read(); }
  float value() const { return m_mapped_adc.value(); }
  const FloatVariable& valueVariable() const { return m_mapped_adc.mapped_value(); }
  int raw_counts() const { return m_mapped_adc.raw_counts(); }

 private:
  static constexpr unsigned kCfgSet = VariableBase::Flags::kConfig | VariableBase::Flags::kSettable;

  MappedAnalogSensor m_mapped_adc;
};

}  // namespace og3
