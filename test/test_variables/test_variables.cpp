// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>
#include <ArduinoJson.h>

#include "og3/constants.h"
#include "og3/html_table.h"
#include "og3/units.h"
#include "og3/variable.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

void test_int_vars() {
  og3::VariableGroup vg("test1");
  og3::Variable<int> ival("ival", 10, "", "test integer", 0, &vg);
  TEST_ASSERT_EQUAL_STRING("ival", ival.name());
  TEST_ASSERT_EQUAL_STRING("test integer", ival.description());
  TEST_ASSERT_EQUAL_STRING("test1", ival.group()->name());
  TEST_ASSERT_EQUAL(10, ival.value());
  ival = -11;
  TEST_ASSERT_EQUAL(-11, ival.value());
  ival.fromString("-12");
  TEST_ASSERT_EQUAL(-12, ival.value());
  ival = 13;
  TEST_ASSERT_EQUAL_STRING("13", ival.string().c_str());
}

void test_unsigned_vars() {
  og3::VariableGroup vg("test2");
  og3::Variable<unsigned> uval("uval", 10, "", "test integer", 0, &vg);
  TEST_ASSERT_EQUAL(10, uval.value());
  uval = 11;
  TEST_ASSERT_EQUAL(11, uval.value());
  uval.fromString("12");
  TEST_ASSERT_EQUAL(12, uval.value());
  uval = 13;
  TEST_ASSERT_EQUAL_STRING("13", uval.string().c_str());
}

void test_float_vars() {
  og3::VariableGroup vg("test3");
  og3::FloatVariable fval("fval", 10.0, "", "test float", 0, 2, &vg);
  TEST_ASSERT_EQUAL_FLOAT(10.0, fval.value());
  fval = 0.5;
  TEST_ASSERT_EQUAL_FLOAT(0.5, fval.value());
  fval.fromString("5.25");
  TEST_ASSERT_EQUAL_FLOAT(5.25, fval.value());
  fval = 10.0;
  TEST_ASSERT_EQUAL_STRING("10.00", fval.string().c_str());
}

void test_double_vars() {
  og3::VariableGroup vg("test4");
  og3::DoubleVariable fval("fval", 10.0, "", "test double", 0, 2, &vg);
  TEST_ASSERT_EQUAL_FLOAT(10.0, fval.value());
  fval = 0.5;
  TEST_ASSERT_EQUAL_FLOAT(0.5, fval.value());
  fval.fromString("5.25");
  TEST_ASSERT_EQUAL_FLOAT(5.25, fval.value());
  fval = 10.0;
  TEST_ASSERT_EQUAL_STRING("10.00", fval.string().c_str());
}

void test_string_vars() {
  og3::VariableGroup vg("test5");
  og3::Variable<String> sval("sval", "default", "", "test string", 0, &vg);
  TEST_ASSERT_EQUAL_STRING("default", sval.value().c_str());
  sval = "11";
  TEST_ASSERT_EQUAL_STRING("11", sval.value().c_str());
  sval.fromString("12");
  TEST_ASSERT_EQUAL_STRING("12", sval.value().c_str());
  sval = "foo";
  TEST_ASSERT_EQUAL_STRING("foo", sval.string().c_str());
}

void test_bool_vars() {
  og3::VariableGroup vg("bool");
  og3::Variable<bool> bval("bval", false, "", "test bool", 0, &vg);
  og3::BoolVariable bbval("bbval", false, "test bool2", 0, &vg);
  TEST_ASSERT_FALSE(bval.value());
  TEST_ASSERT_FALSE(bbval.value());
  bval = true;
  bbval = true;
  TEST_ASSERT_TRUE(bval.value());
  TEST_ASSERT_TRUE(bbval.value());
  bval.fromString("");
  bbval.fromString("");
  TEST_ASSERT_FALSE(bval.value());
  TEST_ASSERT_FALSE(bbval.value());
  bval.fromString("0");
  TEST_ASSERT_FALSE(bval.value());
  bval.fromString("false");
  TEST_ASSERT_FALSE(bval.value());
  bval.fromString("F");
  TEST_ASSERT_FALSE(bval.value());
  bval.fromString("1");
  TEST_ASSERT_TRUE(bval.value());
  bval.fromString("true");
  bbval.fromString("true");
  TEST_ASSERT_TRUE(bval.value());
  TEST_ASSERT_TRUE(bbval.value());
  bval.fromString("boo");
  TEST_ASSERT_TRUE(bval.value());
}

void test_html_table() {
  og3::VariableGroup vg("climate");
  og3::Variable<String> loc("loc", String("home"), "", "location", 0, &vg);
  og3::FloatVariable tempC("tempC", 20.22, og3::units::kCelsius, "temperature", 0, 2, &vg);
  og3::FloatVariable tempF("tempF", tempC.value() + 9 / 5 + 32, og3::units::kFahrenheit,
                           "temperatureF", 0, 1, &vg);
  og3::DoubleVariable humidtiy("humidity", 60.1, og3::units::kPercentage, "humidity", 0, 1, &vg);
  String html;
  og3::html::writeTableInto(&html, vg);
#if 0
  printf("%s\n", html.c_str());
  TEST_ASSERT_TRUE(false);
#endif
}

void test_json_parse() {
  constexpr unsigned kCfgSet =
      og3::VariableBase::Flags::kConfig | og3::VariableBase::Flags::kSettable;
  og3::VariableGroup vg("watering");
  og3::FloatVariable max_moisture_target("max_moisture_target", 80.0f, og3::units::kPercentage,
                                         "Max moisture", kCfgSet, 0, &vg);
  og3::FloatVariable min_moisture_target("min_moisture_target", 70.0f, og3::units::kPercentage,
                                         "Min moisture", kCfgSet, 0, &vg);
  og3::FloatVariable pump_dose_msec("pump_on_msec", 3 * og3::kMsecInSec, og3::units::kMilliseconds,
                                    "Pump on time", kCfgSet, 0, &vg);
  og3::BoolVariable watering_enabled("watering_enabled", false, "watering enabled", kCfgSet, &vg);
  og3::BoolVariable reservoir_check_enabled("res_check_enabled", false, "reservior check enabled",
                                            kCfgSet, &vg);
  og3::FloatVariable pump_seconds_after_low("pump_after_low", 10.0f, og3::units::kSeconds,
                                            "pump seconds after low water", kCfgSet, 0, &vg);
  const char json[] =
      "{\"soil_moisture_in_min\":2900,\"soil_moisture_in_max\":1470,\"soil_moisture_out_min\":0,"
      "\"soil_moisture_out_max\":100,\"soil_moisture_delta_per_deg\":0.075000003,\"max_moisture_"
      "target\":75,\"min_moisture_target\":65,\"pump_on_msec\":3000,\"watering_enabled\":\"true\","
      "\"res_check_enabled\":\"true\",\"pump_after_low\":10}";

  JsonDocument doc;
  deserializeJson(doc, json);
  for (auto* var : vg.variables()) {
    if (!var->config()) {
      continue;
    }
    const JsonVariant val = (doc)[var->name()];
    if (val.isNull()) {
      continue;
    }
    TEST_ASSERT_TRUE(var->fromJson(val));
  }
  TEST_ASSERT_TRUE(reservoir_check_enabled.value());
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_int_vars);
  RUN_TEST(test_unsigned_vars);
  RUN_TEST(test_float_vars);
  RUN_TEST(test_double_vars);
  RUN_TEST(test_string_vars);
  RUN_TEST(test_bool_vars);
  RUN_TEST(test_html_table);
  RUN_TEST(test_json_parse);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduion framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
