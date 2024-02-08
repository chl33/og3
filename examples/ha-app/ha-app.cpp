// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

// These headers help platformio determine dependencies when doing a CI-build.
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
// These are the relevant include files for using the library.
#include <og3/ha_app.h>
#include <og3/html_table.h>

#define SW_VERSION "0.1.0"
#define OTA_PASSWORD "notsecure!"

namespace {

const char kModel[] = "ha-app-test";
const char kSoftware[] = "ha-app-test " SW_VERSION;

// This is an example application with a web server and MQTT client support.
// The MQTT client support includes code for Home Assistant device/entity discovery.
og3::HAApp s_app(
    og3::HAApp::Options("chl33", kModel, kSoftware)
        .withWifiApp(og3::WifiApp::Options()
                         .withApp(og3::App::Options().withLogType(og3::App::LogType::kUdp))
                         .withDefaultDeviceName("ha-test")
                         .withUdpLogHost(IPAddress(192, 168, 0, 20))
                         .withOta(og3::OtaManager::Options(OTA_PASSWORD))));

void handleWebRoot(AsyncWebServerRequest* request) {
  // The send of the web page happens asynchronously after this function exits, so we need to make
  // sure the storage for the page remains.  I don't know how to handle the case where
  // multiple clients are being served data at once.
  static String s_body;
  s_body.clear();
  og3::html::writeTableInto(&s_body, s_app.wifi_manager().variables());
  s_app.wifi_manager().add_html_config_button(&s_body);
  s_app.mqtt_manager().add_html_config_button(&s_body);
  s_app.app_status().addHtmlButton(&s_body);
  og3::sendWrappedHTML(request, s_app.board_cname(), kSoftware, s_body.c_str());
}

}  // namespace

void setup() {
  s_app.setup();
  s_app.web_server().serveStatic("/static/", LittleFS, "/static/").setCacheControl("max-age=600");
  s_app.web_server().on("/", handleWebRoot);
  s_app.web_server().on(og3::WifiManager::kConfigUrl, [](AsyncWebServerRequest* request) {
    s_app.wifi_manager().handleRequest(request, s_app.board_cname(), kSoftware);
  });
  s_app.web_server().on(og3::MqttManager::kConfigUrl, [](AsyncWebServerRequest* request) {
    s_app.mqtt_manager().handleRequest(request, s_app.board_cname(), kSoftware);
  });
}
void loop() { s_app.loop(); }
