// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/module.h"
#include "og3/web.h"
#include "og3/wifi.h"

#if !defined(ESP32)
class AsyncCallbackJsonWebHandler;
#endif

namespace og3 {

#if defined(ESP32)
using NetEndpoint = PsychicEndpoint;
#else
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

/**
 * @brief A module for managing a portable NetServer (PsychicHttpServer or AsyncWebServer).
 *
 * The WebServer module abstracts the underlying networking stack. On ESP32, it
 * automatically configures the server for typical PsychicHttp usage (e.g., max_uri_handlers).
 *
 * Developers should use the `on()` and `onJson()` methods to register portable routes.
 */
class WebServer : public Module {
 public:
  explicit WebServer(ModuleSystem* module_system, uint16_t port = 80);

  static WebServer* get(const NameToModule& n2m) { return GetModule<WebServer>(n2m, kName); }

  static const char* kName;

#ifndef NATIVE
  /** @brief Returns a reference to the underlying server object. */
  NetServer& server() { return m_server; }
  /** @brief Returns a reference to the underlying server object. */
  NetServer& native_server() { return m_server; }

  /**
   * @brief Register a portable GET route.
   * @param uri The path to handle (e.g., "/")
   * @param handler A function taking (NetRequest*, NetResponse*)
   */
  NetEndpoint* on(const char* uri, NetHandler handler);

#if defined(ESP32)
  /** @brief Register a portable route with a specific HTTP method (ESP32). */
  NetEndpoint* on(const char* uri, http_method method, NetHandler handler);
  /** @brief Register a portable route with file upload support (ESP32). */
  NetEndpoint* on(const char* uri, http_method method, NetHandler handler,
                  NetUploadCallback upload_handler);
  /** @brief Register a portable JSON route (ESP32). */
  NetEndpoint* onJson(const char* uri, http_method method, NetJsonHandler handler);
#else
  /** @brief Register a portable route with a specific HTTP method (ESP8266). */
  void on(const char* uri, WebRequestMethod method, NetHandler handler);
  /** @brief Register a portable route with file upload support (ESP8266). */
  void on(const char* uri, WebRequestMethod method, NetHandler handler,
          NetUploadCallback upload_handler);
  /** @brief Register a portable JSON route (ESP8266). */
  void onJson(const char* uri, WebRequestMethod method, NetJsonHandler handler);
#endif

  /** @brief Set the default handler for unknown URIs. */
  void onNotFound(NetHandler handler);
#endif

 private:
#ifndef NATIVE
  NetServer m_server;
#endif
  WifiManager* m_wifi_manager = nullptr;
};

}  // namespace og3
