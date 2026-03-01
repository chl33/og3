// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/module.h"
#include "og3/web.h"
#include "og3/wifi.h"

namespace og3 {

#if defined(ESP32)
using NetEndpoint = PsychicEndpoint;
#else
class AsyncCallbackJsonWebHandler;
using NetEndpoint = void;
#endif

class WifiManager;
class VariableBase;
class VariableGroup;

bool read(NetRequest& request, VariableBase& var);
bool read(NetRequest& request, const VariableGroup& var_group);

// Upload callback signature
using NetUploadCallback =
    std::function<NetHandlerStatus(NetRequest*, const String&, size_t, uint8_t*, size_t, bool)>;

// A module for managing a NetServer (PsychicHttpServer or AsyncWebServer).
class WebServer : public Module {
 public:
  explicit WebServer(ModuleSystem* module_system, uint16_t port = 80);

  static WebServer* get(const NameToModule& n2m) { return GetModule<WebServer>(n2m, kName); }

  static const char* kName;

#ifndef NATIVE
  NetServer& server() { return m_server; }
  NetServer& native_server() { return m_server; }

  // Abstraction for registering routes
  NetEndpoint* on(const char* uri, NetHandler handler);
#if defined(ESP32)
  NetEndpoint* on(const char* uri, http_method method, NetHandler handler);
  NetEndpoint* on(const char* uri, http_method method, NetHandler handler,
                  NetUploadCallback upload_handler);
  NetEndpoint* onJson(const char* uri, http_method method, NetJsonHandler handler);
#else
  void on(const char* uri, WebRequestMethod method, NetHandler handler);
  void on(const char* uri, WebRequestMethod method, NetHandler handler,
          NetUploadCallback upload_handler);
  void onJson(const char* uri, WebRequestMethod method, NetJsonHandler handler);
#endif

  void onNotFound(NetHandler handler);
#endif

 private:
#ifndef NATIVE
  NetServer m_server;
#endif
  WifiManager* m_wifi_manager = nullptr;
};

}  // namespace og3
