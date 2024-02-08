// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/ha_app.h"

namespace og3 {

HAApp::HAApp(const HAApp::Options& options)
    : WebApp(options.app),
      m_mqtt_manager(options.mqtt, &tasks()),
      m_ha_discovery(options.ha_discovery, &module_system()),
      m_app_status(&tasks()),
      m_wifi_monitor(&tasks()) {}

void HAApp::setup() {
  WebApp::setup();
#ifndef NATIVE
  web_server().on(AppStatus::kUrl, [this](AsyncWebServerRequest* request) {
    m_app_status.handleRequest(request, ha_discovery().options().software);
  });
#endif
}

}  // namespace og3
