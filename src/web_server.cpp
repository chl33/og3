// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/web_server.h"

#include "og3/config_interface.h"
#include "og3/wifi_manager.h"

namespace og3 {

const char* WebServer::kName = "web_server";

bool read(const AsyncWebServerRequest& request, VariableBase& var) {
#ifndef NATIVE
  if (!request.hasParam(var.name(), true)) {
    return false;
  }
  return var.fromString(request.getParam(var.name(), true)->value().c_str());
#else
  return true;
#endif
}
bool read(const AsyncWebServerRequest& request, const VariableGroup& var_group) {
#ifndef NATIVE
  bool ret = true;
  for (auto* var : var_group.variables()) {
    if (var->settable()) {
      ret = read(request, *var) && ret;
    }
  }
  return ret;
#else
  return true;
#endif
}

WebServer::WebServer(ModuleSystem* module_system, uint16_t port)
    : Module(WebServer::kName, module_system)
#ifndef NATIVE
      ,
      m_server(port)
#endif
{
  add_link_fn([this](NameToModule& name_to_module) -> bool {
    m_wifi_manager = WifiManager::get(name_to_module);
    if (!m_wifi_manager) {
      return false;
    }
#ifndef NATIVE
    m_wifi_manager->addConnectCallback([this]() {
      m_server.begin();
      log()->log("Web server started.");
    });
#endif
    return true;
  });
}

}  // namespace og3
