// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>
#include <og3/adc.h>
#include <og3/logger.h>
#include <og3/mapped_analog_sensor.h>
#include <og3/module.h>
#include <og3/module_system.h>

#include "unity.h"

namespace og3 {

namespace {

class TestLogger : public Logger {
 public:
  virtual void log(const char* msg) final {
    m_output += msg;
    m_output += "\n";
  }
  bool check(const std::string& expected) {
    const bool ret = m_output == expected;
    if (!ret) {
      printf(
          "Expected '%s'\n"
          "but got  '%s'\n",
          expected.c_str(), m_output.c_str());
    }
    m_output.clear();
    return ret;
  }

 private:
  std::string m_output;
};

}  // namespace

}  // namespace og3

void setUp() { ArduinoFakeReset(); }

void tearDown() {}

void test1() {
  og3::TestLogger log;
  og3::Logger* plog = &log;
  og3::ModuleSystem modules(&plog);
  og3::VariableGroup vg("vg");
  og3::Adc adc("test", A0, &modules, "desc", 0, vg);

  using namespace fakeit;
  When(Method(ArduinoFake(), analogRead)).Return(0, 10);

  TEST_ASSERT_EQUAL_UINT(0, adc.read());
  TEST_ASSERT_EQUAL_UINT(10, adc.read());
  Verify(Method(ArduinoFake(), analogRead)).Exactly(2_Times);
}

void test2() {
  og3::TestLogger log;
  og3::Logger* plog = &log;
  og3::ModuleSystem modules(&plog);
  og3::VariableGroup vg("vg");
  og3::VariableGroup cvg("cvg");

  og3::MappedAnalogSensor mas(
      {
          .name = "test",
          .units = "%",
          .description = "fake moisture",
          .default_in_min = 1024,
          .default_in_max = 2048,
          .default_out_min = 0.0f,
          .default_out_max = 1.0f,
          .valid_in_min = 0,
          .valid_in_max = 2050,
      },
      &modules, cvg, vg);

  using namespace fakeit;
  When(Method(ArduinoFake(), analogRead)).Return(1024, 2048, (1024 + 2048) / 2, 0, 2050, 4000);

  JsonDocument json;
  TEST_ASSERT_EQUAL_FLOAT(0.0f, mas.read());
  TEST_ASSERT_EQUAL_FLOAT(0.0f, mas.value());
  TEST_ASSERT_FALSE(mas.readingIsFailed());
  mas.mapped_value().toJson(&json);
  {
    og3::VariableGroup tvg("tmp");
    og3::FloatVariable out("out", 1.0f, "", "", 0, 0, tvg);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, out.value());
    TEST_ASSERT_TRUE(out.fromJson(json[mas.name()]));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, out.value());
  }
  TEST_ASSERT_EQUAL_UINT(1024, mas.raw_counts());
  TEST_ASSERT_EQUAL_FLOAT(1.0f, mas.read());
  TEST_ASSERT_EQUAL_FLOAT(0.5f, mas.read());
  TEST_ASSERT_EQUAL_FLOAT(0.0f, mas.read());
  TEST_ASSERT_EQUAL_FLOAT(1.0f, mas.read());
  TEST_ASSERT_EQUAL_FLOAT(0.0f, mas.read());
  TEST_ASSERT_TRUE(mas.readingIsFailed());
  Verify(Method(ArduinoFake(), analogRead)).Exactly(6_Times);
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test1);
  RUN_TEST(test2);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduion framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
