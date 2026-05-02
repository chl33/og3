// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/adc.h"

namespace og3 {

/**
 * @brief Performs a linear mapping from raw ADC counts to a physical unit (e.g. voltage, moisture).
 *
 * This class handles reading an analog pin, validating the input range, and applying
 * a configurable linear transform (y = mx + b). Parameters for the mapping are
 * stored as persistent configuration variables.
 */
class MappedAnalogSensor {
 public:
  MappedAnalogSensor(const MappedAnalogSensor&) = delete;
  MappedAnalogSensor(MappedAnalogSensor&&) = delete;
  MappedAnalogSensor& operator=(const MappedAnalogSensor&) = delete;
  MappedAnalogSensor& operator=(MappedAnalogSensor&&) = delete;

  /** @brief Configuration options for the mapped sensor. */
  struct Options {
    const char* name;             ///< Unique name for the sensor.
    uint8_t pin;                  ///< The hardware ADC pin.
    const char* units;            ///< Units for the output value.
    const char* raw_description;  ///< Description for the raw count variable.
    const char* description;      ///< Description for the mapped output variable.
    unsigned raw_var_flags;       ///< Variable flags for the raw counts.
    unsigned mapped_var_flags;    ///< Variable flags for the mapped output.
    unsigned config_flags;        ///< Variable flags for the config parameters.
    int default_in_min;           ///< Default raw value for the minimum calibration point.
    int default_in_max;           ///< Default raw value for the maximum calibration point.
    float default_out_min;        ///< Default unit value for the minimum calibration point.
    float default_out_max;        ///< Default unit value for the maximum calibration point.
    int config_decimals;          ///< Precision for config values.
    int decimals;                 ///< Precision for output values.
    unsigned valid_in_min;        ///< Minimum raw value considered valid (error otherwise).
    unsigned valid_in_max;        ///< Maximum raw value considered valid (error otherwise).
  };

  /** @brief Constructs a MappedAnalogSensor. */
  MappedAnalogSensor(const Options& options, ModuleSystem* module_system_, VariableGroup& cfgvg,
                     VariableGroup& vg);

  /** @brief Manually maps a raw value using current calibration. */
  float map(int inval) const;

  /**
   * @brief Reads the ADC and updates the mapped value.
   * @return The updated mapped value.
   */
  float read();

  /** @return The name of the mapped value variable. */
  const char* name() { return m_mapped_value.name(); }
  /** @return The current mapped physical value. */
  float value() const { return m_mapped_value.value(); }
  /** @return The current raw ADC counts. */
  int raw_counts() const { return m_adc.counts(); }

  /** @brief Checks if a specific count value is within the hardware-valid range. */
  bool readingIsBad(unsigned counts) const {
    return counts < m_valid_in_min || counts > m_valid_in_max;
  }
  /** @return true if the last ADC reading was invalid or failed. */
  bool readingIsFailed() const { return readingIsBad(m_adc.counts()); }

  /** @brief Manually updates the input calibration minimum. */
  void set_in_min(int in_min) { m_in_min = in_min; }
  /** @brief Manually updates the input calibration maximum. */
  void set_in_max(int in_max) { m_in_max = in_max; }
  /** @brief Manually updates the output unit minimum. */
  void set_out_min(float out_min) { m_out_min = out_min; }
  /** @brief Manually updates the output unit maximum. */
  void set_out_max(float out_max) { m_out_max = out_max; }

  /** @return Current input calibration minimum. */
  int in_min() const { return m_in_min.value(); }
  /** @return Current input calibration maximum. */
  int in_max() const { return m_in_max.value(); }
  /** @return Current output unit minimum. */
  float out_min() const { return m_out_min.value(); }
  /** @return Current output unit maximum. */
  float out_max() const { return m_out_max.value(); }

  /** @return Reference to the raw ADC count variable. */
  Variable<unsigned>& raw_value() { return m_adc.countsVar(); }
  /** @return Constant reference to the raw ADC count variable. */
  const Variable<unsigned>& raw_value() const { return m_adc.countsVar(); }

  /** @return Reference to the mapped unit variable. */
  FloatVariable& mapped_value() { return m_mapped_value; }
  /** @return Constant reference to the mapped unit variable. */
  const FloatVariable& mapped_value() const { return m_mapped_value; }

 private:
  const unsigned m_valid_in_min;
  const unsigned m_valid_in_max;
  String m_name;
  String m_raw_name;
  String m_in_max_name;
  String m_in_min_name;
  String m_out_max_name;
  String m_out_min_name;
  Adc m_adc;
  Variable<int> m_in_min;
  Variable<int> m_in_max;
  FloatVariable m_out_min;
  FloatVariable m_out_max;
  FloatVariable m_mapped_value;
};

}  // namespace og3
