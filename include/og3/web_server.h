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

#ifndef NATIVE
bool read(const AsyncWebServerRequest& request, VariableBase& var);
bool read(const AsyncWebServerRequest& request, const VariableGroup& var_group);

// A module for managing a AsyncWebServer.
class WebServer : public Module {
 public:
  explicit WebServer(ModuleSystem* module_system, uint16_t port = 80);

  static const char* kName;

  AsyncWebServer& server() { return m_server; }

 private:
  AsyncWebServer m_server;
  WifiManager* m_wifi_manager = nullptr;
};
#endif

}  // namespace og3
