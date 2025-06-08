// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/adc_voltage.h"

#include "og3/units.h"

namespace og3 {

AdcVoltage::AdcVoltage(const char* name, WifiApp* app, uint8_t pin, const char* raw_desc,
                       const char* desc, float out_max, VariableGroup& vg, VariableGroup& config_vg)
    : ConfigModule(name, app),
      m_mapped_adc(
          {
              .name = name,
              .pin = pin,
              .units = units::kVolt,
              .raw_description = raw_desc,
              .description = desc,
              .raw_var_flags = 0,
              .mapped_var_flags = 0,
              .config_flags = kCfgSet,
              .default_in_min = 0,
              .default_in_max = 1 << 12,
              .default_out_min = 0.0f,
              .default_out_max = out_max,
              .config_decimals = 2,
              .decimals = 2,
              .valid_in_min = 50,
              .valid_in_max = 1 << 12,
          },
          &app->module_system(), config_vg, vg) {}

}  // namespace og3
