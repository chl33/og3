// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/module.h"

namespace og3 {

class FlashSupport;
class Logger;
class VariableGroup;

/**
 * @brief The ConfigInterface module supports saving selected variables (those with flag kConfig) in
 * a VariableGroup to a configuration file in flash storage and load them again.
 */
class ConfigInterface : public Module {
 public:
  static const char* kName;  ///< Module name

  /**
   * @brief Construct a new ConfigInterface object.
   * @param module_system The module system instance.
   */
  explicit ConfigInterface(ModuleSystem* module_system);

  /**
   * @brief Read configuration from a file into a variable group.
   * @param var_group The variable group to load variables into.
   * @param filename The filename to read from (optional).
   * @return true if reading succeeded.
   */
  bool read_config(VariableGroup& var_group, const char* filename = nullptr);

  /**
   * @brief Write configuration from a variable group to a file.
   * @param var_group The variable group to save variables from.
   * @param filename The filename to write to (optional).
   * @return true if writing succeeded.
   */
  bool write_config(const VariableGroup& var_group, const char* filename = nullptr);

  /**
   * @brief Get the application logger.
   * @return Pointer to the logger.
   */
  Logger* log();

 private:
  FlashSupport* m_fs = nullptr;
};

}  // namespace og3
