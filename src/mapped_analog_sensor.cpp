// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/mapped_analog_sensor.h"

namespace og3 {

MappedAnalogSensor::MappedAnalogSensor(const Options& options, ModuleSystem* module_system_,
                                       VariableGroup& cfg_vg, VariableGroup& vg)
    : m_valid_in_min(options.valid_in_min),
      m_valid_in_max(options.valid_in_max),
      m_name(options.name),
      m_raw_name(m_name + "_raw"),
      m_in_max_name(m_name + "_in_max"),
      m_in_min_name(m_name + "_in_min"),
      m_out_max_name(m_name + "_out_max"),
      m_out_min_name(m_name + "_out_min"),
      m_adc(m_raw_name.c_str(), options.pin, module_system_, options.raw_description,
            options.raw_var_flags, vg),
      m_in_min(m_in_min_name.c_str(), options.default_in_min, "counts", "min ADC reading",
               options.config_flags, cfg_vg),
      m_in_max(m_in_max_name.c_str(), options.default_in_max, "counts", "max ADC reading",
               options.config_flags, cfg_vg),
      m_out_min(m_out_min_name.c_str(), options.default_out_min, options.units, "min out val",
                options.config_flags, options.config_decimals, cfg_vg),
      m_out_max(m_out_max_name.c_str(), options.default_out_max, options.units, "max out val",
                options.config_flags, options.config_decimals, cfg_vg),
      m_mapped_value(m_name.c_str(), options.default_out_min, options.units, options.description,
                     options.mapped_var_flags, options.decimals, vg) {
  m_mapped_value.setFailed();
}

float MappedAnalogSensor::map(int inval) const {
  const float slope =
      (m_out_max.value() - m_out_min.value()) / (m_in_max.value() - m_in_min.value());
  const float out = (inval - m_in_min.value()) * slope + m_out_min.value();
  if (out < m_out_min.value()) {
    return m_out_min.value();
  } else if (out > m_out_max.value()) {
    return m_out_max.value();
  } else {
    return out;
  }
}

float MappedAnalogSensor::read() {
  const unsigned raw_val = m_adc.read();
  if (readingIsFailed()) {
    m_mapped_value.setFailed();
    return m_out_min.value();
  }
  const float value = this->map(raw_val);
  m_mapped_value = value;
  m_mapped_value.setFailed(false);
  return value;
}

}  // namespace og3
