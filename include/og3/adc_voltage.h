// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.
#pragma once

#include <og3/app.h>
#include <og3/config_module.h>
#include <og3/mapped_analog_sensor.h>
#include <og3/variable.h>

namespace og3 {

/**
 * @brief A module which represents an ADC voltage reading with some scaling factor.
 *
 * This assumes that a reading of zero counts translates to zero volts.
 */
class AdcVoltage : public ConfigModule {
 public:
  /**
   * @brief Construct a new AdcVoltage object.
   * @param name The name of the module.
   * @param app The application instance.
   * @param pin The ADC pin to read from.
   * @param raw_desc Description for the raw ADC value.
   * @param desc Description for the mapped voltage value.
   * @param out_max The voltage when the ADC returns its maximum count value.
   * @param vg The variable group to add the voltage variable to.
   * @param config_vg The variable group to add the configuration variables to.
   */
  AdcVoltage(const std::string& name, App* app, uint8_t pin, const char* raw_desc, const char* desc,
             float out_max, VariableGroup& vg, VariableGroup& config_vg);

  /**
   * @brief Read the current voltage from the ADC.
   * @return The current voltage.
   */
  float read() { return m_mapped_adc.read(); }

  /**
   * @brief Get the last read voltage.
   * @return The last read voltage.
   */
  float value() const { return m_mapped_adc.value(); }

  /**
   * @brief Get the voltage variable.
   * @return The voltage variable.
   */
  const FloatVariable& valueVariable() const { return m_mapped_adc.mapped_value(); }

  /**
   * @brief Get the last raw ADC counts.
   * @return The last raw ADC counts.
   */
  int raw_counts() const { return m_mapped_adc.raw_counts(); }

 private:
  static constexpr unsigned kCfgSet = VariableBase::Flags::kConfig | VariableBase::Flags::kSettable;

  MappedAnalogSensor m_mapped_adc;
};

}  // namespace og3
