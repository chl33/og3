// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.
#pragma once

#include "og3/app.h"
#include "og3/config_interface.h"
#include "og3/dependencies.h"
#include "og3/module.h"
#include "og3/variable.h"
#include "og3/web_server.h"
#include "og3/wifi_app.h"

namespace og3 {

// ConfigModule is a parent class for modules which save and load their variable values
//  from flash via the config system and should have their own web page for setting
//  variable values.
class ConfigModule : public Module {
 public:
  ConfigModule(const char* name, App* app);
  const char* cfg_url() const { return m_cfg_url.c_str(); }
  void add_html_button(String* body) const;

 protected:
  const App* m_app;
  DependenciesArray<2> m_config_module_deps;
  VariableGroup m_cvg;
  ConfigInterface* m_config = nullptr;
  WebServer* m_web_server = nullptr;
  std::string m_cfg_url;
  String m_html;

 private:
  void handleConfigRequest(AsyncWebServerRequest* request);
};

}  // namespace og3
