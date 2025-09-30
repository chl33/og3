// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/wifi_manager.h"

#include <functional>

#ifndef NATIVE
#include <DNSServer.h>
#endif

#include "og3/config_interface.h"
#include "og3/constants.h"
#include "og3/html_table.h"
#include "og3/units.h"
#include "og3/web.h"
#include "og3/web_server.h"
#include "og3/wifi.h"

namespace og3 {

namespace {
#ifndef NATIVE
constexpr uint8_t DNS_PORT = 53;
#endif
const IPAddress apSoftIP(192, 168, 4, 1);

static const char kStatusUninitialized[] = "init";
static const char kStatusNoShield[] = "no-shield";
static const char kStatusIdle[] = "idle";
static const char kStatusNoSSIDAvail[] = "no-SSID-avail";
static const char kStatusCompleted[] = "completed";
static const char kStatusConnected[] = "connected";
static const char kStatusConnectFailed[] = "connect-failed";
static const char kStatusConnectionLost[] = "connection-lost";
static const char kStatusDisconnected[] = "disconnected";
static const char kStatusWrongPassword[] = "wrong-password";

const char* nullToEmpty(const char* str) { return str ? str : ""; }
}  // namespace

#ifndef NATIVE
const char* strWifiStatus(wl_status_t status) {
  if (status == kWiFiStatusNone) {
    status = WiFi.status();
  }
  switch (status) {
    case WL_NO_SHIELD:
      return kStatusNoShield;
    case WL_IDLE_STATUS:
      return kStatusIdle;
    case WL_NO_SSID_AVAIL:
      return kStatusNoSSIDAvail;
    case WL_SCAN_COMPLETED:
      return kStatusCompleted;
    case WL_CONNECTED:
      return kStatusConnected;
    case WL_CONNECT_FAILED:
      return kStatusConnectFailed;
    case WL_CONNECTION_LOST:
      return kStatusConnectionLost;
    case WL_DISCONNECTED:
      return kStatusDisconnected;
#ifndef ARDUINO_ARCH_ESP32
    case WL_WRONG_PASSWORD:
      return kStatusWrongPassword;
#endif
  }
  return "?unknown?";
}
#endif

const char WifiManager::kName[] = "wifi";
const char WifiManager::kConfigUrl[] = "/wifi/update";

WifiManager::WifiManager(const char* default_board_name, Tasks* tasks,
                         const WifiManager::Options& options)
    : Module(WifiManager::kName, tasks->module_system()),
      m_dependencies(ConfigInterface::kName),
      m_scheduler(tasks),
      m_sanity_scheduler(tasks),
      m_ap_password(options.ap_password),
      m_vg(kName, nullptr, 4),
      m_board("board", default_board_name, "", "Device name",
              VariableBase::kConfig | VariableBase::kSettable, m_vg),
      m_essid("essid", nullToEmpty(options.default_essid), "", "",
              VariableBase::kConfig | VariableBase::kSettable | VariableBase::kNoPublish, m_vg),
      m_password("password", nullToEmpty(options.default_password), "", "",
                 VariableBase::kConfig | VariableBase::kSettable | VariableBase::kNoPublish |
                     VariableBase::kNoDisplay,
                 m_vg),
      m_ip_addr("ip", "", "", "", 0, m_vg),
      m_rssi("rssi", 0, units::kDecibel, "", 0, m_vg) {
  setDependencies(&m_dependencies);
  add_link_fn([this](og3::NameToModule& name_to_module) -> bool {
    m_config = ConfigInterface::get(name_to_module);
    return m_config != nullptr;
  });
  add_init_fn([this]() {
#ifndef NATIVE
    WiFi.persistent(false);  // Keep the esp from automatically writing essid+paswd to flash.
#endif
    if (m_config) {
      m_config->read_config(m_vg);
      if (m_board.value().length() == 0) {
        m_board = "og3board";
      }
    }
#ifdef ARDUINO_ARCH_ESP32
    m_wifiEventIdConnected = WiFi.onEvent(
        [this](arduino_event_id_t event, arduino_event_info_t info) { onWifiEvent(event, info); });
#elif defined(ARDUINO_ARCH_ESP8266)
    m_wifiConnectHandler =
        WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP& event) { onConnect(); });
    m_wifiDisconnectHandler = WiFi.onStationModeDisconnected(
        [this](const WiFiEventStationModeDisconnected& event) { onDisconnect(); });
#endif
  });
  add_start_fn([this]() {
    if (m_enable) {
      trySetup();
    }
  });
#ifndef NATIVE
  add_update_fn([this]() {
    if (m_ap_mode && m_dns_server) {
      m_dns_server->processNextRequest();
    }
  });
#endif
  scheduleSanityCheck();
}

void WifiManager::onConnect() {
  m_start_connect_msec = 0;
  m_disconnect_msec = 0;
#ifndef NATIVE
  const IPAddress ip = m_ap_mode ? WiFi.softAPIP() : WiFi.localIP();
  m_ip_addr = ip.toString().c_str();
  log()->logf("Connected to Wi-Fi. IP address: %s", m_ip_addr.value().c_str());
  for (const auto& callback : m_connectCallbacks) {
    callback();
  }
#endif
  m_was_connected = true;
}

void WifiManager::onDisconnect() {
  if (m_was_connected) {
#ifndef NATIVE
    log()->logf("Wifi: onDisconnect(%s): was connected.", strWifiStatus());
#endif
#ifndef NATIVE
    WiFi.disconnect();  // disconnect so we can run Wifi.begin() later to attempt re-connection.
#endif
    for (const auto& callback : m_disconnectCallbacks) {
      callback();
    }
    // Try to reconnect after 1 minute.
    m_scheduler.runIn(kMsecInMin, [this]() { trySetup(); });
    m_disconnect_msec = millis();
  } else {
#ifndef NATIVE
    log()->debugf("Wifi: onDisconnect(%s): was not connected.", strWifiStatus());
#endif
  }
  m_was_connected = false;
}

void WifiManager::trySetup() {
#ifndef NATIVE
  // If we are waiting for a connection, wait if it has been less than 30 seconds since
  //  we started the connection attempt.
  if (m_start_connect_msec != 0) {
    const auto now_msec = millis();
    const auto wait_end_msec = m_start_connect_msec + 30 * 1000;
    if (wait_end_msec > now_msec && m_start_connect_msec < now_msec /*integer wraparound*/) {
      const auto wait_secs = 1e-3 * (wait_end_msec - now_msec);
      log()->debugf("Wifi setup -- waiting %.1f more seconds", wait_secs);
      m_scheduler.runIn(wait_end_msec - now_msec, [this]() { trySetup(); });
      return;
    }
  }
  log()->debugf("Wifi: setup (%s)", strWifiStatus());

  // Take some action based on the current status.
  switch (WiFi.status()) {
    case WL_NO_SHIELD:
      // My esp32 module returns no_shield even though it supports WiFi.
      startClient();
      return;
    case WL_CONNECTED:  // Already connected.
      return;
    case WL_IDLE_STATUS:
    case WL_SCAN_COMPLETED:
    case WL_CONNECTION_LOST:
    case WL_DISCONNECTED:
      startClient();
      return;
    case WL_NO_SSID_AVAIL:
    case WL_CONNECT_FAILED:
#ifndef ARDUINO_ARCH_ESP32
    case WL_WRONG_PASSWORD:
#endif
      startAp();
      return;
  }
#endif
}

void WifiManager::scheduleSanityCheck() {
  m_sanity_scheduler.runIn(kMsecInHour, [this]() { this->sanityCheck(); });
}

// Every hour, see whether WiFi is off and we should retry.
void WifiManager::sanityCheck() {
#ifndef NATIVE
  // Has it been an hour since we last disconnected?
  auto disconnect_timeout = [this]() {
    const auto now = millis();
    if (m_disconnect_msec == 0) {
      return false;  // not disconnected??
    }
    if (now < m_disconnect_msec) {
      return true;  // millis() wrapped-around since last disconnection.
    }
    return (now - m_disconnect_msec) >= kMsecInHour;
  };

  if (!m_ap_mode && WiFi.status() != WL_CONNECTED && disconnect_timeout()) {
    log()->log("Wifi: sanity check -- restarting connection");
    trySetup();
  } else {
    log()->debug("Wifi: sanity check -- no action");
  }
  scheduleSanityCheck();
#endif
}

// This is a function to call to startup the board in AP mode.
void WifiManager::startAp() {
#ifndef NATIVE
  const char* essid = (board().length() > 0) ? board().c_str() : "og3board";
  log()->logf("Wifi: starting in AP mode (%s).", essid);
  WiFi.mode(WIFI_AP);
  m_ap_mode = true;
  if (!WiFi.softAPConfig(apSoftIP, apSoftIP, IPAddress(255, 255, 255, 0))) {
    log()->log("Failed to setup soft-AP config");
    return;
  }
  if (m_ap_password) {
    if (!WiFi.softAP(essid, m_ap_password)) {
      log()->log("Failed to setup SoftAP mode (with password).");
      return;
    }
  } else {
    if (!WiFi.softAP(essid)) {
      log()->log("Failed to setup SoftAP mode.");
      return;
    }
  }
  log()->debugf("Started AP mode as %s", essid);
  if (!m_dns_server) {
    m_dns_server.reset(new DNSServer());
  }
  m_dns_server->setErrorReplyCode(DNSReplyCode::NoError);
  m_dns_server->start(DNS_PORT, "*", apSoftIP);
  for (const auto& callback : m_softAPCallbacks) {
    callback();
  }
  // Allow time to setup maybe??
  m_scheduler.runIn(100, [this]() { onConnect(); });
  m_start_connect_msec = millis();
#endif
}

// This is a function to startup the board as a Wifi client (if configured).
void WifiManager::startClient() {
#ifndef NATIVE
  // If we don't have a configured ESSID then startup in AP mode instead.
  const bool have_essid = essid().length() > 0;
  if (!have_essid) {
    startAp();
    return;
  }
  // If we have a configured ESSID, try to connect to it.
  m_ap_mode = false;
  m_start_connect_msec = millis();
  log()->logf("Wifi: connecting to essid %s (%s)", essid().c_str(), strWifiStatus());
  WiFi.mode(WIFI_STA);
  WiFi.begin(essid().c_str(), password().c_str());
#endif
}

#ifdef ARDUINO_ARCH_ESP32
void WifiManager::onWifiEvent(arduino_event_id_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:
      log()->log("WiFi interface ready");
      break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
      log()->log("Completed scan for access points");
      break;
    case ARDUINO_EVENT_WIFI_STA_START:
      log()->log("WiFi client started");
      break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
      log()->log("WiFi clients stopped");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      log()->log("Connected to access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      m_scheduler.runIn(1, [this]() { onDisconnect(); });
      break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
      log()->log("Authentication mode of access point has changed");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      m_scheduler.runIn(1, [this]() { onConnect(); });
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      log()->log("Lost IP address and IP address is reset to 0");
      break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      log()->log("WiFi Protected Setup (WPS): succeeded in enrollee mode");
      break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
      log()->log("WiFi Protected Setup (WPS): failed in enrollee mode");
      break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
      log()->log("WiFi Protected Setup (WPS): timeout in enrollee mode");
      break;
    case ARDUINO_EVENT_WPS_ER_PIN:
      log()->log("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case ARDUINO_EVENT_WIFI_AP_START:
      log()->log("WiFi access point started");
      break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
      log()->log("WiFi access point  stopped");
      break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      log()->log("Client connected");
      break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      log()->log("Client disconnected");
      break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
      log()->log("Assigned IP address to client");
      break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
      log()->log("Received probe request");
      break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
      log()->log("AP IPv6 is preferred");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
      log()->log("STA IPv6 is preferred");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
      log()->log("Ethernet IPv6 is preferred");
      break;
    case ARDUINO_EVENT_ETH_START:
      log()->log("Ethernet started");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      log()->log("Ethernet stopped");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      log()->log("Ethernet connected");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      log()->log("Ethernet disconnected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      log()->log("Obtained IP address");
      break;
    case ARDUINO_EVENT_WIFI_FTM_REPORT:
    case ARDUINO_EVENT_WPS_ER_PBC_OVERLAP:
    case ARDUINO_EVENT_SC_SCAN_DONE:
    case ARDUINO_EVENT_SC_FOUND_CHANNEL:
    case ARDUINO_EVENT_SC_GOT_SSID_PSWD:
    case ARDUINO_EVENT_SC_SEND_ACK_DONE:
    case ARDUINO_EVENT_PROV_INIT:
    case ARDUINO_EVENT_PROV_DEINIT:
    case ARDUINO_EVENT_PROV_START:
    case ARDUINO_EVENT_PROV_END:
    case ARDUINO_EVENT_PROV_CRED_RECV:
    case ARDUINO_EVENT_PROV_CRED_FAIL:
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
    case ARDUINO_EVENT_MAX:
      break;
  }
}
#endif

void WifiManager::updateStatus() {
#ifndef NATIVE
  m_rssi = WiFi.RSSI();
#endif
}

}  // namespace og3
