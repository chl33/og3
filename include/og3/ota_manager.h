// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/wifi_manager.h"

namespace og3 {

// This is a wrapper to support over-the-air firmware updates.
class OtaManager : public Module {
 public:
  static const char kName[];

  struct Options {
    Options(const char* default_password_ = nullptr) : default_password(default_password_) {}
    const char* default_password;
  };

  OtaManager(const Options& opts, ModuleSystem* module_system);

 private:
  void setup();
  void enable();

  const Options m_opts;
  DependenciesArray<2> m_dependencies;
  VariableGroup m_vg;
  Variable<std::string> m_password;
  ConfigInterface* m_config = nullptr;
  WifiManager* m_wifi_manager = nullptr;

  unsigned long m_last_progress_msec = 0;
};

}  // namespace og3
