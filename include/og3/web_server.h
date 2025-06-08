// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/module.h"
#include "og3/web.h"
#include "og3/wifi.h"

namespace og3 {

class WifiManager;
class VariableBase;
class VariableGroup;

bool read(const AsyncWebServerRequest& request, VariableBase& var);
bool read(const AsyncWebServerRequest& request, const VariableGroup& var_group);

// A module for managing a AsyncWebServer.
class WebServer : public Module {
 public:
  explicit WebServer(ModuleSystem* module_system, uint16_t port = 80);

  static WebServer* get(const NameToModule& n2m) { return GetModule<WebServer>(n2m, kName); }

  static const char* kName;

#ifndef NATIVE
  AsyncWebServer& server() { return m_server; }
#endif

 private:
#ifndef NATIVE
  AsyncWebServer m_server;
#endif
  WifiManager* m_wifi_manager = nullptr;
};

}  // namespace og3
