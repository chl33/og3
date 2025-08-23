// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/adc.h"

namespace og3 {

// A MappedAnalogSensor performs a linear mapping from integer ADC values to floating point outputs.
// The mapping parameters are config variables.
// The input and output values are variables which can be monitored.
class MappedAnalogSensor {
 public:
  struct Options {
    const char* name;
    uint8_t pin;                  // The ADC input
    const char* units;            // Units for the output values
    const char* raw_description;  // Variable description for the raw input values
    const char* description;      // Variable description for the mapped output values
    unsigned raw_var_flags;       // Variable flags for the input values
    unsigned mapped_var_flags;    // Variable flags for the output values
    unsigned config_flags;        // Variable flags for config variables
    int default_in_min;           // Default value for the minimum ADC input value
    int default_in_max;           // Default value for the maximum ADC input value
    float default_out_min;        // Default value mapped output value of the minimum input value
    float default_out_max;        // Default value mapped output value of the minimum output value
    int config_decimals;          // Printed decimal values of config floats
    int decimals;                 // Printed decimal values of output values
    unsigned valid_in_min;        // If the input reads less than this, the reading is invalid.
    unsigned valid_in_max;        // If the input reads more than this, the reading is invalid.
  };

  MappedAnalogSensor(const Options& options, ModuleSystem* module_system_, VariableGroup& cfgvg,
                     VariableGroup& vg);

  float map(int inval) const;
  float read();

  const char* name() { return m_mapped_value.name(); }
  float value() const { return m_mapped_value.value(); }
  int raw_counts() const { return m_adc.counts(); }
  bool readingIsBad(unsigned counts) const {
    return counts < m_valid_in_min || counts > m_valid_in_max;
  }
  bool readingIsFailed() const { return readingIsBad(m_adc.counts()); }
  void set_in_min(int in_min) { m_in_min = in_min; }
  void set_in_max(int in_max) { m_in_max = in_max; }
  void set_out_min(int out_min) { m_out_min = out_min; }
  void set_out_max(int out_max) { m_out_max = out_max; }

  Variable<unsigned>& raw_value() { return m_adc.countsVar(); }
  const Variable<unsigned>& raw_value() const { return m_adc.countsVar(); }

  FloatVariable& mapped_value() { return m_mapped_value; }
  const FloatVariable& mapped_value() const { return m_mapped_value; }

 private:
  const unsigned m_valid_in_min;
  const unsigned m_valid_in_max;
  std::string m_name;
  std::string m_raw_name;
  std::string m_in_max_name;
  std::string m_in_min_name;
  std::string m_out_max_name;
  std::string m_out_min_name;
  Adc m_adc;
  Variable<int> m_in_min;
  Variable<int> m_in_max;
  FloatVariable m_out_min;
  FloatVariable m_out_max;
  FloatVariable m_mapped_value;
};

}  // namespace og3
