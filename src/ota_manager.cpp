// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/ota_manager.h"

#include <Arduino.h>
#ifndef NATIVE
#include <ArduinoOTA.h>
#endif

#include "og3/config_interface.h"
#include "og3/wifi_manager.h"

namespace og3 {

const char OtaManager::kName[] = "ota";

OtaManager::OtaManager(const Options& opts, ModuleSystem* module_system)
    : Module(kName, module_system),
      m_opts(opts),
      m_dependencies({WifiManager::kName, ConfigInterface::kName}),
      m_vg(kName),
      m_password("password", opts.default_password, "", "password",
                 VariableBase::kConfig | VariableBase::kSettable, m_vg) {
  setDependencies(&m_dependencies);
  add_link_fn([this](NameToModule& name_to_module) -> bool {
    m_config = ConfigInterface::get(name_to_module);
    m_wifi_manager = WifiManager::get(name_to_module);
    return m_config && m_wifi_manager;
  });
  add_init_fn([this]() {
    if (!m_config || !m_wifi_manager) {
      return;
    }
    m_config->read_config(m_vg);
    setup();
  });
#ifndef NATIVE
  add_update_fn([this]() { ArduinoOTA.handle(); });
#endif
}

void OtaManager::setup() {
#ifndef NATIVE
  if (m_password.value().length() == 0) {
    return;
  }
  ArduinoOTA.setPassword(m_password.value().c_str());
  ArduinoOTA.onStart([this]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    log()->logf("OTA: Start updating %s", type.c_str());
  });
  ArduinoOTA.onEnd([&]() { log()->log("End"); });
  ArduinoOTA.onProgress([&](unsigned int progress, unsigned int total) {
    unsigned long now = millis();
    if (now - m_last_progress_msec >= 1000) {
      const int percent = progress / (total / 100);
      log()->logf("Progress: %u%%", percent);
      m_last_progress_msec = now;
    }
  });
  ArduinoOTA.onError([&](ota_error_t error) {
    Serial.printf("OTA: Error[%u]: ", error);
    switch (error) {
      case OTA_AUTH_ERROR:
        log()->log("OTA: Auth Failed");
        break;
      case OTA_BEGIN_ERROR:
        log()->log("OTA: Begin Failed");
        break;
      case OTA_CONNECT_ERROR:
        log()->log("OTA: Connect Failed");
        break;
      case OTA_RECEIVE_ERROR:
        log()->log("OTA: Receive Failed");
        break;
      case OTA_END_ERROR:
        log()->log("OTA: End Failed");
        break;
    }
  });
#endif
  m_wifi_manager->addConnectCallback(std::bind(&OtaManager::enable, this));
}

void OtaManager::enable() {
#ifndef NATIVE
  const String hostname = m_wifi_manager->board() + ".local";
  ArduinoOTA.setHostname(hostname.c_str());
  ArduinoOTA.begin();
  log()->log("OTA ready");
#endif
}

}  // namespace og3
