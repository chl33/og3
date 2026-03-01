// Copyright (c) 2026 Chris Lee and contibuters.
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
  WebServer& web_server_module() { return m_web_server; }
#endif

  NetHandlerStatus handleWifiConfigRequest(NetRequest* request);
#ifndef NATIVE
  WebButton createWifiConfigButton();
  WebButton createRestartButton();
#endif

 protected:
#ifndef NATIVE
  WebServer m_web_server;
  String m_web_page;  // Stores html while asyncwebserver sends the data.
#endif
};

}  // namespace og3
