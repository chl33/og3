// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

// These headers help platformio determine dependencies when doing a CI-build.
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
// This is the relevant include file for using the library.
#include <og3/html_table.h>
#include <og3/web_app.h>

#define SW_VERSION "0.1.0"

namespace {

const char* kSoftware = "web-app" SW_VERSION;

// Select which pin to use for blinking depending on the kind of board we are using.
#if defined(BLINK_LED)
constexpr uint8_t kLEDPin = BLINK_LED;
#elif defined(ARDUINO_ARCH_ESP32)
constexpr uint8_t kLEDPin = 7;
#elif defined(ARDUINO_ARCH_ESP8266)
constexpr uint8_t kLEDPin = D4;
#endif

// WebApp adds support for web interfacs to the basic wifi-app shown in examples/wifi-app.
og3::WebApp s_app(og3::WifiApp::Options()
                      .withSoftwareName(kSoftware)
                      .withDefaultDeviceName("web-test")
                      .withApp(og3::App::Options().withLogType(og3::App::LogType::kSerial)));

// This module is responsible for blinking the LED (see examples/blink/blink.cpp).
class Blink : public og3::Module {
 public:
  explicit Blink(og3::App* app) : og3::Module("blink", &app->module_system()), m_app(app) {
    add_init_fn([this]() { pinMode(kLEDPin, OUTPUT); });
    add_start_fn([this]() { blink(); });
  }
  void blink() {
    digitalWrite(kLEDPin, m_high ? HIGH : LOW);
    m_high = !m_high;
    m_app->tasks().runIn(1000, [this]() { blink(); });
  }

 private:
  og3::App* m_app;
  bool m_high = false;
};

Blink s_blink(&s_app);
og3::WebButton s_button_wifi_config = s_app.createWifiConfigButton();
og3::WebButton s_button_restart = s_app.createRestartButton();

void handleWebRoot(AsyncWebServerRequest* request) {
  // The send of the web page happens asynchronously after this function exits, so we need to make
  // sure the storage for the page remains.  I don't know how to handle the case where
  // multiple clients are being served data at once.
  static String s_body;
  s_body.clear();
  // Show wifi-state on the root page of the web app.
  og3::html::writeTableInto(&s_body, s_app.wifi_manager().variables());
  // Add a button for a web form, for configuring the board's Wifi settings.
  s_button_wifi_config.add_button(&s_body);
  // Add a button for rebooting the board.
  s_button_restart.add_button(&s_body);
  // Send the rendered web page to the client.
  og3::sendWrappedHTML(request, s_app.board_cname(), kSoftware, s_body.c_str());
}

}  // namespace

void setup() {
  s_app.setup();
  s_app.web_server().on("/", handleWebRoot);
}
void loop() { s_app.loop(); }
