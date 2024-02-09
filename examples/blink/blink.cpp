// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

// These headers help platformio determine dependencies when doing a CI-build.
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <LittleFS.h>
// This is the relevant include file for using the library.
#include <og3/app.h>

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
og3::App s_app(og3::App::Options().withLogType(og3::App::LogType::kSerial));

// This module is responsible for blinking the LED (1 second off, 1 second on, ...).
class Blink : public og3::Module {
 public:
  explicit Blink(og3::App* app) : og3::Module("blink", &app->module_system()), m_app(app) {
    // During app initialization, set the LED pin to output mode.
    add_init_fn([this]() { pinMode(kLEDPin, OUTPUT); });
    // When the app starts, start blinking.
    add_start_fn([this]() { blink(); });
  }
  // The blink function is first called, on start(), and afterward it is called via a
  //  timed task.
  // After toggling the LED output pin, we schedule the function to be called again in
  //  one second (1000 msec), then we log the state of the output pin to the serial port.
  void blink() {
    digitalWrite(kLEDPin, m_high ? HIGH : LOW);
    m_high = !m_high;
    m_app->tasks().runIn(1000, [this]() { blink(); });
    m_app->log().logf("blink: %s", m_high ? "on" : "off");
  }

 private:
  og3::App* m_app;
  bool m_high = false;
};

Blink s_blink(&s_app);

}  // namespace

// The Arduino setup() and loop() functions just defer to the App,
//  letting the `ModuleSystem` setup the modules, and the `Tasks` system
//  run scheduled callback functions.
void setup() { s_app.setup(); }
void loop() { s_app.loop(); }
