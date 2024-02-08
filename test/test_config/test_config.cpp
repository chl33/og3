// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>

#include "og3/config_interface.h"
#include "og3/flash_support.h"
#include "og3/logger.h"
#include "og3/module_system.h"
#include "og3/units.h"
#include "og3/variable.h"
#include "unity.h"

namespace {
class StdoutLogger : public og3::Logger {
  void log(const char* msg) final { printf("%s\n", msg); }
};
}  // namespace

void setUp() {}

void tearDown() {}

void test_config() {
  og3::VariableGroup vg("climate");
  og3::Variable<String> loc("loc", String("home"), "", "location", og3::VariableBase::kConfig, &vg);
  og3::FloatVariable tempC("tempC", 20.22, og3::units::kCelsius, "temperature",
                           og3::VariableBase::kConfig, 2, &vg);
  og3::FloatVariable tempF("tempF", tempC.value() + 9 / 5 + 32, og3::units::kFahrenheit,
                           "temperatureF", 0, 1, &vg);
  og3::DoubleVariable humidity("humidity", 60.1, og3::units::kPercentage, "humidity",
                               og3::VariableBase::kConfig, 1, &vg);
  StdoutLogger log;
  og3::ModuleSystem ds(&log);
  og3::FlashSupport flash(&ds);
  og3::ConfigInterface config(&ds);

  ds.setup();
  config.write_config(vg, "test.cfg");
  tempC = 0.0f;
  TEST_ASSERT_EQUAL_FLOAT(0.0f, tempC.value());
  tempF = 0.0f;
  TEST_ASSERT_EQUAL_FLOAT(0.0f, tempF.value());
  humidity = 0.0f;
  TEST_ASSERT_EQUAL_FLOAT(0.0f, humidity.value());
  loc = "bogo";
  TEST_ASSERT_EQUAL_STRING("bogo", loc.value().c_str());

  config.read_config(&vg, "test.cfg");
  TEST_ASSERT_EQUAL_FLOAT(20.22f, tempC.value());
  TEST_ASSERT_EQUAL_FLOAT(0.0f, tempF.value());
  TEST_ASSERT_EQUAL_FLOAT(60.1f, humidity.value());
  TEST_ASSERT_EQUAL_STRING("home", loc.value().c_str());
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_config);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduino framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
