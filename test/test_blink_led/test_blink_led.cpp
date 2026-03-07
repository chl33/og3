// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>

#include "og3/app.h"
#include "og3/blink_led.h"
#include "unity.h"

using namespace fakeit;

void setUp() { ArduinoFakeReset(); }
void tearDown() {}

void test_blink_led_basic() {
  og3::App app({});
  uint8_t pin = 13;

  When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
  When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();

  og3::BlinkLed led("myled", pin, &app, 100, false /*onLow*/);

  app.setup();
  Verify(Method(ArduinoFake(), pinMode).Using(pin, OUTPUT)).Once();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Once();  // off()

  led.on();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Once();

  led.off();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Exactly(2);
}

void test_blink_sequence() {
  og3::App app({});
  uint8_t pin = 13;

  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
  When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
  When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();

  og3::BlinkLed led("myled", pin, &app, 100, false /*onLow*/);
  app.setup();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Once();  // Initial off

  // blink(2) -> on, then off after 100ms, then on after 100ms, then off after 100ms
  led.blink(2);
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Once();

  // t=100ms: should turn OFF
  When(Method(ArduinoFake(), millis)).AlwaysReturn(100);
  app.tasks().loop();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Exactly(2);  // Initial + this one

  // t=200ms: should turn ON again (2nd blink)
  When(Method(ArduinoFake(), millis)).AlwaysReturn(200);
  app.tasks().loop();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Exactly(2);  // Previous + this one

  // t=300ms: should turn OFF finally
  When(Method(ArduinoFake(), millis)).AlwaysReturn(300);
  app.tasks().loop();
  Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW))
      .Exactly(3);  // Initial + 1st off + 2nd off
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_blink_led_basic);
  RUN_TEST(test_blink_sequence);
  return UNITY_END();
}

int main() { return runUnityTests(); }

void setup() {}
void loop() {}
void app_main() { runUnityTests(); }
