// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>

#include "og3/html_table.h"
#include "og3/units.h"
#include "og3/variable.h"
#include "unity.h"

void setUp() { ArduinoFakeReset(); }
void tearDown() {}

void test_html_escape() {
  String out;
  og3::html::escape(&out, "test & < > %");
  TEST_ASSERT_EQUAL_STRING("test &amp; &lt; &gt; &percnt;", out.c_str());

  out = "";
  og3::html::escape(&out, nullptr);
  TEST_ASSERT_EQUAL_STRING("", out.c_str());
}

void test_html_table() {
  og3::VariableGroup vg("test_vg");
  og3::FloatVariable v1("var1", 12.3f, og3::units::kCelsius, "Temperature", 0, 1, vg);
  og3::Variable<int> v2("var2", 42, og3::units::kWatt, "Power", 0, vg);

  String out;
  og3::html::writeTableInto(&out, vg);

  if (!strstr(out.c_str(), "var1")) {
    printf("Actual output: %s\n", out.c_str());
  }

  // Basic structural checks
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "<table class=\"readings\">"));
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "Temperature"));
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "12.3"));
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "°C"));
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "Power"));
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "42"));
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "W"));
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "</table>"));
}

void test_html_form() {
  og3::VariableGroup vg("config_vg");
  // Only settable variables should appear in the form
  og3::FloatVariable v1("settable_var", 1.0f, "", "Setting", og3::VariableBase::kSettable, 1, vg);
  og3::Variable<int> v2("readonly_var", 100, "", "Readonly", 0, vg);

  String out;
  og3::html::writeFormTableInto(&out, vg);

  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "settable_var"));
  TEST_ASSERT_NULL(strstr(out.c_str(), "readonly_var"));
  TEST_ASSERT_NOT_NULL(strstr(out.c_str(), "<button name='save' type='submit'"));
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_html_escape);
  RUN_TEST(test_html_table);
  RUN_TEST(test_html_form);
  return UNITY_END();
}

int main() { return runUnityTests(); }

void setup() {}
void loop() {}
void app_main() { runUnityTests(); }
