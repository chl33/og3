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
  // For captive portal mode, temporarily route unknown URI paths to the WiFi config page
  // while in Soft AP mode, and restore the normal default handler upon STA reconnect.
#if defined(ESP32)
  wifi_manager().addSoftAPCallback([this]() {
    auto& server = web_server_module().native_server();
    if (!m_softap_endpoint) {
      m_softap_endpoint = std::make_unique<PsychicEndpoint>(&server, HTTP_ANY, "");
      auto* handler = new PsychicWebHandler();
      handler->onRequest([this](NetRequest* request, NetResponse* response) {
        return handleWifiConfigRequest(request, response);
      });
      m_softap_endpoint->setHandler(handler);
    }
    if (server.defaultEndpoint != m_softap_endpoint.get()) {
      m_saved_default_endpoint = server.defaultEndpoint;
      server.defaultEndpoint = m_softap_endpoint.get();
    }
  });
  wifi_manager().addConnectCallback([this]() {
    if (!wifi_manager().apMode() && m_saved_default_endpoint) {
      auto& server = web_server_module().native_server();
      if (server.defaultEndpoint == m_softap_endpoint.get()) {
        server.defaultEndpoint = m_saved_default_endpoint;
      }
      m_saved_default_endpoint = nullptr;
    }
  });
#else
  wifi_manager().addSoftAPCallback([this]() {
    web_server_module().onNotFound([this](NetRequest* request, NetResponse* response) {
      if (wifi_manager().apMode()) {
        return handleWifiConfigRequest(request, response);
      }
      request->send(404, "text/plain", "Not found");
      NET_REPLY(request, ESP_OK);
    });
  });
#endif
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
