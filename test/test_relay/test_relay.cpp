// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>

#include "og3/app.h"
#include "og3/relay.h"
#include "og3/variable.h"
#include "unity.h"

using namespace fakeit;

void setUp() { ArduinoFakeReset(); }
void tearDown() {}

void test_relay_basic() {
  og3::App app({});
  og3::VariableGroup vg("test_vg");
  uint8_t pin = 5;

  When(Method(ArduinoFake(), millis)).AlwaysReturn(1000);
  When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();

  og3::Relay relay("myrelay", &app.tasks(), pin, "desc", true, vg, og3::Relay::OnLevel::kHigh);

  // Constructor calls turnOff()
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Once();
  TEST_ASSERT_FALSE(relay.isOn());

  // turnOn
  relay.turnOn();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Once();
  TEST_ASSERT_TRUE(relay.isOn());
  TEST_ASSERT_EQUAL(1000, relay.lastOnMsec());

  // turnOff
  relay.turnOff();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW))
      .Exactly(2);  // constructor + this call
  TEST_ASSERT_FALSE(relay.isOn());
  TEST_ASSERT_EQUAL(1000, relay.lastOnMsec());
}

void test_relay_low_active() {
  og3::App app({});
  og3::VariableGroup vg("test_vg");
  uint8_t pin = 5;

  When(Method(ArduinoFake(), millis)).AlwaysReturn(1000);
  When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();

  og3::Relay relay("myrelay", &app.tasks(), pin, "desc", true, vg, og3::Relay::OnLevel::kLow);

  // Constructor calls turnOff() -> set(true) for kLow active
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Once();
  TEST_ASSERT_FALSE(relay.isOn());

  // turnOn -> set(false)
  relay.turnOn();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Once();
  TEST_ASSERT_TRUE(relay.isOn());

  // turnOff -> set(true)
  relay.turnOff();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Exactly(2);
  TEST_ASSERT_FALSE(relay.isOn());
}

void test_relay_timed() {
  og3::App app({});
  og3::VariableGroup vg("test_vg");
  uint8_t pin = 5;

  When(Method(ArduinoFake(), millis)).AlwaysReturn(1000);
  When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();

  og3::Relay relay("myrelay", &app.tasks(), pin, "desc", true, vg, og3::Relay::OnLevel::kHigh);

  bool callback_called = false;
  relay.turnOn(500, [&callback_called]() { callback_called = true; });

  TEST_ASSERT_TRUE(relay.isOn());
  TEST_ASSERT_FALSE(callback_called);

  // Check tasks
  TEST_ASSERT_FALSE(app.tasks().queue().empty());
  TEST_ASSERT_EQUAL(1000 + 500, app.tasks().queue().first().msec);

  // Run tasks
  When(Method(ArduinoFake(), millis)).AlwaysReturn(1500);
  app.tasks().loop();

  TEST_ASSERT_FALSE(relay.isOn());
  TEST_ASSERT_TRUE(callback_called);
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_relay_basic);
  RUN_TEST(test_relay_low_active);
  RUN_TEST(test_relay_timed);
  return UNITY_END();
}

int main() { return runUnityTests(); }

void setup() {}
void loop() {}
void app_main() { runUnityTests(); }
