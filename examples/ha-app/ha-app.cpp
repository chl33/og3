// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

// These headers help platformio determine dependencies when doing a CI-build.
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
// These are the relevant include files for using the library.
#include <og3/constants.h>
#include <og3/ha_app.h>
#include <og3/ha_dependencies.h>
#include <og3/html_table.h>

#define SW_VERSION "0.1.0"
#define OTA_PASSWORD "notsecure!"

namespace {

const char kModel[] = "ha-app-test";
const char kSoftware[] = "ha-app-test " SW_VERSION;

// Select which pin to use for blinking depending on the kind of board we are using.
#if defined(BLINK_LED)
constexpr uint8_t kLEDPin = BLINK_LED;
#elif defined(ARDUINO_ARCH_ESP32)
constexpr uint8_t kLEDPin = 7;
#elif defined(ARDUINO_ARCH_ESP8266)
constexpr uint8_t kLEDPin = D4;
#endif

// This is an example application with a web server and MQTT client support.
// The MQTT client support includes code for Home Assistant device/entity discovery.
og3::HAApp s_app(
    og3::HAApp::Options("chl33", kModel, kSoftware)
        .withWifiApp(og3::WifiApp::Options()
                         .withApp(og3::App::Options().withLogType(og3::App::LogType::kSerial))
                         .withDefaultDeviceName("ha-test")
                         .withUdpLogHost(IPAddress(192, 168, 0, 20))
                         .withOta(og3::OtaManager::Options(OTA_PASSWORD))));

// This module is responsible for blinking the LED (10 second off, 10 second on, ...).
class Blink : public og3::Module {
 public:
  explicit Blink(og3::App* app)
      : og3::Module("blink", &app->module_system()),
        m_app(app),
        m_vg("blink", 2),
        m_high("led_on", false, "LED is on", 0, &m_vg),
        // Blink every 10 second, starting at first millisecond.
        m_blink_timing(
            1, 10 * og3::kMsecInSec, [this]() { blink(); }, &app->tasks()) {
    setDependencies(&m_dependencies);  // Depend on MQTT & HADiscovery, & get pointers to them.
    add_init_fn([this]() {
      // During app initialization, set the LED pin to output mode.
      pinMode(kLEDPin, OUTPUT);
      // Declare the blink status as a sensor in Home Assistant via its MQTT Discovery protocol.
      if (m_dependencies.ok()) {
        m_dependencies.ha_discovery()->addDiscoveryCallback(
            [this](og3::HADiscovery* had, JsonDocument* json) -> bool {
              json->clear();
              // Declare this this as a binary sensor of type light.
              return had->addMeas(json, m_high, og3::ha::device_type::kBinarySensor,
                                  og3::ha::device_class::binary_sensor::kLight);
            });
      }
    });
  }

  const og3::VariableGroup& variables() const { return m_vg; }

 private:
  void blink() {
    digitalWrite(kLEDPin, m_high.value() ? HIGH : LOW);
    m_high = !m_high.value();
    m_app->log().logf("blink: %s", m_high.value() ? "on" : "off");
    if (m_dependencies.mqtt_manager()) {
      m_dependencies.mqtt_manager()->mqttSend(m_vg);
    }
  }

  og3::App* m_app;                            // A pointer to the App.
  og3::HADependencies m_dependencies;         // This module depends on MQTTManager and HADisovery
  og3::VariableGroup m_vg;                    // The variable group for the module variables
  og3::BinarySensorVariable m_high;           // The only module variable: the status of the LED
  og3::PeriodicTaskScheduler m_blink_timing;  // Controls of blink timing
};

Blink s_blink(&s_app);

void handleWebRoot(AsyncWebServerRequest* request) {
  // The send of the web page happens asynchronously after this function exits, so we need to
  // make sure the storage for the page remains.  I don't know how to handle the case where
  // multiple clients are being served data at once.
  static String s_body;
  s_body.clear();
  og3::html::writeTableInto(&s_body, s_blink.variables());
  og3::html::writeTableInto(&s_body, s_app.wifi_manager().variables());
  og3::html::writeTableInto(&s_body, s_app.mqtt_manager().variables());
  s_app.wifi_manager().add_html_config_button(&s_body);
  s_app.mqtt_manager().add_html_config_button(&s_body);
  s_app.app_status().addHtmlButton(&s_body);
  s_body += F(HTML_BUTTON("/restart", "Restart"));
  og3::sendWrappedHTML(request, s_app.board_cname(), kSoftware, s_body.c_str());
}

}  // namespace

void setup() {
  // For captive portal mode, map unknown URI paths to the root page.
  s_app.wifi_manager().addSoftAPCallback([]() {
    s_app.web_server().onNotFound([](AsyncWebServerRequest* request) {
      s_app.wifi_manager().handleRequest(request, s_app.board_cname(), kSoftware);
    });
  });
  s_app.setup();
  s_app.web_server().serveStatic("/static/", LittleFS, "/static/").setCacheControl("max-age=600");
  s_app.web_server().on("/", handleWebRoot);
  s_app.web_server().on(og3::WifiManager::kConfigUrl, [](AsyncWebServerRequest* request) {
    s_app.wifi_manager().handleRequest(request, s_app.board_cname(), kSoftware);
  });
  s_app.web_server().on(og3::MqttManager::kConfigUrl, [](AsyncWebServerRequest* request) {
    s_app.mqtt_manager().handleRequest(request, s_app.board_cname(), kSoftware);
  });
  s_app.web_server().on("/restart", [](AsyncWebServerRequest* request) {
    og3::htmlRestartPage(request, &s_app.tasks());
  });
}
void loop() { s_app.loop(); }
