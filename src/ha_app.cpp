// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <og3/ha_app.h>
#include <og3/html_table.h>

namespace og3 {

HAApp::HAApp(const HAApp::Options& options)
    : WebApp(options.app),
      m_mqtt_manager(options.mqtt, &tasks()),
      m_ha_discovery(options.ha_discovery, &module_system()),
      m_app_status(&tasks()),
      m_wifi_monitor(&tasks()) {}

void HAApp::setup() { WebApp::setup(); }

void HAApp::handleMqttConfigRequest(AsyncWebServerRequest* request) {
#ifndef NATIVE
  ::og3::read(*request, mqtt_manager().mutableVariables());
  m_web_page.clear();
  html::writeFormTableInto(&m_web_page, mqtt_manager().variables());
  m_web_page += F(HTML_BUTTON("/", "Back"));
  sendWrappedHTML(request, board_cname(), software_name(), m_web_page.c_str());
  config().write_config(mqtt_manager().variables());
#endif
}

#ifndef NATIVE
WebButton HAApp::createMqttConfigButton() {
  return WebButton(&web_server(), "MQTT Config", MqttManager::kConfigUrl,
                   [this](AsyncWebServerRequest* request) { handleMqttConfigRequest(request); });
}
#endif

void HAApp::handleAppStatusRequest(AsyncWebServerRequest* request) {
#ifndef NATIVE
  m_web_page.clear();
  html::writeTableInto(&m_web_page, app_status().variables());
  m_web_page += F(HTML_BUTTON("/", "Back"));
  sendWrappedHTML(request, board_cname(), software_name(), m_web_page.c_str());
  config().write_config(app_status().variables());
#endif
}

#ifndef NATIVE
WebButton HAApp::createAppStatusButton() {
  return WebButton(&web_server(), "App Status", AppStatus::kUrl,
                   [this](AsyncWebServerRequest* request) { handleAppStatusRequest(request); });
}
#endif

}  // namespace og3
