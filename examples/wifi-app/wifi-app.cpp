// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

// These headers help platformio determine dependencies when doing a CI-build.
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
// This is the relevant include file for using the library.
#include <og3/wifi_app.h>

namespace {

// Select which pin to use for blinking depending on the kind of board we are using.
#if defined(BLINK_LED)
constexpr uint8_t kLEDPin = BLINK_LED;
#elif defined(ARDUINO_ARCH_ESP32)
constexpr uint8_t kLEDPin = 7;
#elif defined(ARDUINO_ARCH_ESP8266)
constexpr uint8_t kLEDPin = D4;
#endif

// Here we create the application object and specify logging to the serial port.
// For a WiFi app, a board name, essid, and password may be set in /wifi.json in flash storage,
//  in which case the application will attempt to connect to the specified Wifi network.
// If Wifi configuration is not setup or if the connection fails, then the board will act as
//  an access point using its board name as the ESSID.
// For web-apps (see examples/web-app/web-app.cpp), the board name, ESSID, and password can be
//  updated via a web form from the boards web interface.
og3::WifiApp s_app(og3::WifiApp::Options()
                       .withSoftwareName("wifi-app")
                       .withDefaultDeviceName("wifi-test")  // default name of board (mDNS, etc..)
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

}  // namespace

// The Arduino setup() and loop() functions just defer to the App,
//  letting the `ModuleSystem` setup the modules, and the `Events` system
//  run scheduled callback functions.
void setup() { s_app.setup(); }
void loop() { s_app.loop(); }
