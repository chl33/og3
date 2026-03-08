// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/web.h"
#include "og3/web_server.h"
#include "og3/wifi_app.h"

namespace og3 {

/**
 * @brief A WifiApp extended with an integrated web server.
 *
 * WebApp provides built-in handlers for WiFi configuration and system
 * restart via a web interface. It automatically creates and manages
 * the WebServer module.
 */
class WebApp : public WifiApp {
 public:
  /** @brief Constructs a WebApp instance. */
  explicit WebApp(const WifiApp::Options& options);

#ifndef NATIVE
  /** @return Reference to the WebServer module. */
  WebServer& web_server_module() { return m_web_server; }
#endif

  /** @brief Web handler for the WiFi configuration form. */
  NetHandlerStatus handleWifiConfigRequest(NetRequest* request, NetResponse* response);

#ifndef NATIVE
  /** @return A button linking to the WiFi config page. */
  WebButton createWifiConfigButton();
  /** @return A button linking to the device restart handler. */
  WebButton createRestartButton();
#endif

 protected:
#ifndef NATIVE
  WebServer m_web_server;
  String m_web_page;  ///< Internal buffer for rendering web pages.
#endif
};

}  // namespace og3
