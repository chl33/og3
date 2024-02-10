// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/web.h"
#include "og3/web_server.h"
#include "og3/wifi_app.h"

namespace og3 {

// A WifiApp with a webserver.
class WebApp : public WifiApp {
 public:
  explicit WebApp(const WifiApp::Options& options);

#ifndef NATIVE
  AsyncWebServer& web_server() { return m_web_server.server(); }
#endif

  void handleWifiConfigRequest(AsyncWebServerRequest* request);
  WebButton createWifiConfigButton();
  WebButton createRestartButton();

 protected:
#ifndef NATIVE
  WebServer m_web_server;
  String m_web_page;  // Stores html while asyncwebserver sends the data.
#endif
};

}  // namespace og3
