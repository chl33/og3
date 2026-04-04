// Copyright (c) 2026 Chris Lee and contibuters.
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
#if defined(ESP32)
  web_server_module().native_server().serveStatic("/static/", LittleFS, "/static/");
#else
  web_server_module()
      .native_server()
      .serveStatic("/static/", LittleFS, "/static/")
      .setCacheControl("max-age=600");
#endif
  // For captive portal mode, map unknown URI paths to the root page.
  wifi_manager().addSoftAPCallback([this]() {
#if defined(ESP32)
    web_server_module().onNotFound([this](NetRequest* request, NetResponse* response) {
      return handleWifiConfigRequest(request, response);
    });
#else
    web_server_module().onNotFound([this](NetRequest* request, NetResponse* response) {
      return handleWifiConfigRequest(request, response);
    });
#endif
  });
}
#endif

NetHandlerStatus WebApp::handleWifiConfigRequest(NetRequest* request, NetResponse* response) {
#ifndef NATIVE
  const bool all_set = ::og3::read(*request, wifi_manager().variables());
  config().write_config(wifi_manager().variables());
  m_web_page.clear();
  // Reboot the board after wifi is configured.
  if (all_set) {
    htmlRestartPage(request, response, &tasks());
    NET_REPLY(request, ESP_OK);
  }

  html::writeFormTableInto(&m_web_page, wifi_manager().variables());
  m_web_page += HTML_BUTTON("/", "Back");
  sendWrappedHTML(request, response, board_cname(), software_name(), m_web_page.c_str());
#endif
  NET_REPLY(request, ESP_OK);
}

#ifndef NATIVE
WebButton WebApp::createWifiConfigButton() {
  return WebButton(&web_server_module().native_server(), "WiFi Config", WifiManager::kConfigUrl,
                   [this](NetRequest* request, NetResponse* response) {
                     return handleWifiConfigRequest(request, response);
                   });
}

WebButton WebApp::createRestartButton() {
  return WebButton(&web_server_module().native_server(), "Restart", "/restart",
                   [this](NetRequest* request, NetResponse* response) {
                     htmlRestartPage(request, response, &tasks());
                     NET_REPLY(request, ESP_OK);
                   });
}
#endif

}  // namespace og3
