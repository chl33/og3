// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/web_app.h"

#ifndef NATIVE
#include <LittleFS.h>
#endif

#include "og3/html_table.h"

namespace og3 {

#ifdef NATIVE
WebApp::WebApp(const WifiApp::Options& options) : WifiApp(options) {}
#else
WebApp::WebApp(const WifiApp::Options& options) : WifiApp(options), m_web_server(&module_system()) {
  // Serve files in flash from the /static/ subdirectory, such as CSS files.
  web_server().serveStatic("/static/", LittleFS, "/static/").setCacheControl("max-age=600");
  // For captive portal mode, map unknown URI paths to the root page.
  wifi_manager().addSoftAPCallback([this]() {
    web_server().onNotFound(
        [this](AsyncWebServerRequest* request) { handleWifiConfigRequest(request); });
  });
}
#endif

void WebApp::handleWifiConfigRequest(AsyncWebServerRequest* request) {
#ifndef NATIVE
  ::og3::read(*request, wifi_manager().mutableVariables());
  m_web_page.clear();
  html::writeFormTableInto(&m_web_page, wifi_manager().variables());
  m_web_page += F(HTML_BUTTON("/", "Back"));
  sendWrappedHTML(request, board_cname(), software_name(), m_web_page.c_str());
  config().write_config(wifi_manager().variables());
#endif
}

#ifndef NATIVE
WebButton WebApp::createWifiConfigButton() {
  return WebButton(&web_server(), "WiFi Config", WifiManager::kConfigUrl,
                   [this](AsyncWebServerRequest* request) { handleWifiConfigRequest(request); });
}

WebButton WebApp::createRestartButton() {
  return WebButton(&web_server(), "Restart", "/restart",
                   [this](AsyncWebServerRequest* request) { htmlRestartPage(request, &tasks()); });
}
#endif

}  // namespace og3
