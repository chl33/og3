// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/ntp.h"

#include <time.h>

#include "og3/wifi_manager.h"

#ifndef NATIVE
#include <Arduino.h>
#ifdef ESP32
#include "esp_sntp.h"
#elif defined(ARDUINO_ARCH_ESP8266)
#include <core_esp8266_features.h>
#endif
#endif

namespace og3 {

const char Ntp::kName[] = "ntp";

#ifdef ARDUINO_ARCH_ESP8266
static Ntp* s_ntp_instance = nullptr;
extern "C" void s_on_time_set() {
  if (s_ntp_instance) {
    s_ntp_instance->updateStatus();
  }
}
#endif

Ntp::Ntp(ModuleSystem* module_system, const Options& options)
    : Module(kName, module_system),
      m_vg(kName),
      m_server1("server1", options.server1, "", "NTP server 1", VariableBase::kConfig, m_vg),
      m_server2("server2", options.server2, "", "NTP server 2", VariableBase::kConfig, m_vg),
      m_server3("server3", options.server3, "", "NTP server 3", VariableBase::kConfig, m_vg),
      m_tz_info("tz", options.tz_info, "", "Timezone string", VariableBase::kConfig, m_vg),
      m_synced("synced", false, "", "NTP synced", 0, m_vg) {
  require(WifiManager::kName, &m_wifi_manager);

#ifdef ARDUINO_ARCH_ESP8266
  s_ntp_instance = this;
#endif

  add_init_fn([this]() {
    if (m_wifi_manager) {
      m_wifi_manager->addConnectCallback([this]() { onWifiConnect(); });
    }
#ifdef ARDUINO_ARCH_ESP8266
    settimeofday_cb(s_on_time_set);
#endif
  });

  // Periodically check sync status (every 10 seconds)
  add_update_fn([this]() {
    static unsigned long last_check = 0;
    if (millis() - last_check > 10000) {
      last_check = millis();
      updateStatus();
    }
  });
}

void Ntp::onWifiConnect() {
#ifndef NATIVE
  log()->logf("NTP: Synchronizing with %s...", m_server1.value().c_str());

  // Set the timezone and start sync
  configTzTime(m_tz_info.value().c_str(), m_server1.value().c_str(), m_server2.value().c_str(),
               m_server3.value().c_str());
#endif
}

void Ntp::updateStatus() {
  bool synced = false;
#ifndef NATIVE
#ifdef ESP32
  synced = (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED);
#else
  // On ESP8266 and others, check if year is valid (> 2020)
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  synced = (timeinfo->tm_year > (2020 - 1900));
#endif
#else
  // Mock behavior for NATIVE
  synced = false;
#endif

  if (synced != m_synced.value()) {
    m_synced = synced;
    if (synced) {
      time_t now = time(nullptr);
      char* time_str = ctime(&now);
      if (time_str) {
        size_t len = strlen(time_str);
        if (len > 0 && time_str[len - 1] == '\n') time_str[len - 1] = '\0';
        log()->logf("NTP: Synced. Current time: %s", time_str);
      }
    }
  }
}

}  // namespace og3
