// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/module.h"

namespace og3 {

class FlashSupport;
class Logger;
class VariableGroup;

// The ConfigInterface module supports saving selected variables (those with flag kConfig) in a
//  VariableGroup to a configuration file in flash storage and load them again.
class ConfigInterface : public Module {
 public:
  static const char* kName;  // module name

  explicit ConfigInterface(ModuleSystem* module_system) RT3_NONNULL();
  bool read_config(VariableGroup* var_group, const char* filename = nullptr) RT3_NONNULL((1));
  bool write_config(const VariableGroup& var_group, const char* filename = nullptr)
      RT3_NONNULL((1));

  Logger* log();

  static ConfigInterface* get(const NameToModule& n2m) {
    return GetModule<ConfigInterface>(n2m, kName);
  }

 private:
  FlashSupport* m_fs = nullptr;
  SingleDependency m_dependency;
};

}  // namespace og3
