// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>

#include "og3/pid.h"
#include "og3/variable.h"
#include "unity.h"

using namespace fakeit;

void setUp() { ArduinoFakeReset(); }
void tearDown() {}

void test_pid_p() {
  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

  og3::VariableGroup vg("vg");
  og3::VariableGroup cfg_vg("cfg_vg");
  og3::VariableGroup cmd_vg("cmd_vg");
  og3::PID::Gains gains(2.0f);
  og3::PID pid(gains, vg, cfg_vg, cmd_vg);

  pid.setTarget(10.0f);
  pid.initialize();

  // error = 10 - 5 = 5. P-cmd = 5 * 2 = 10.
  float cmd = pid.command(5.0f, 1000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, 10.0f, cmd);

  // error = 10 - 12 = -2. P-cmd = -2 * 2 = -4.
  cmd = pid.command(12.0f, 2000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, -4.0f, cmd);
}

void test_pid_i() {
  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

  og3::VariableGroup vg("vg");
  og3::VariableGroup cfg_vg("cfg_vg");
  og3::VariableGroup cmd_vg("cmd_vg");
  og3::PID::Gains gains(0.0f, 1.0f);  // P=0, I=1
  og3::PID pid(gains, vg, cfg_vg, cmd_vg);

  pid.setTarget(10.0f);
  pid.initialize();

  unsigned long start_t = 1000;
  pid.command(10.0f, start_t);  // error=0, no integral accumulation

  // t=2000 (dt=1s), error = 10 - 5 = 5.
  // integral = 0 + 5 * 1s * 1.0 = 5.
  float cmd = pid.command(5.0f, 2000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, 5.0f, cmd);

  // t=3000 (dt=1s), error = 10 - 5 = 5.
  // integral = 5 + 5 * 1s * 1.0 = 10.
  cmd = pid.command(5.0f, 3000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, 10.0f, cmd);
}

void test_pid_d() {
  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

  og3::VariableGroup vg("vg");
  og3::VariableGroup cfg_vg("cfg_vg");
  og3::VariableGroup cmd_vg("cmd_vg");
  og3::PID::Gains gains(0.0f, 0.0f, 0.5f);  // P=0, I=0, D=0.5
  og3::PID pid(gains, vg, cfg_vg, cmd_vg);

  pid.setTarget(10.0f);
  pid.initialize();

  // d_target = 0, d_measured = 2.0. d_error = -2.0.
  // cmd = -2.0 * 0.5 = -1.0.
  float cmd = pid.command(5.0f, 2.0f, 1000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, -1.0f, cmd);
}

void test_pid_saturation() {
  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

  og3::VariableGroup vg("vg");
  og3::VariableGroup cfg_vg("cfg_vg");
  og3::VariableGroup cmd_vg("cmd_vg");
  og3::PID::Gains gains = og3::PID::Gains(20.0f, 1.0f)  // High P to ensure saturation
                              .withCommandMax(10.0f)
                              .withCommandMin(-10.0f);
  og3::PID pid(gains, vg, cfg_vg, cmd_vg);

  pid.setTarget(10.0f);
  pid.initialize();

  // First call: m_command was 0.
  // command_is_saturated() will return false.
  // I will update: 0 + 10 * 1s * 1.0 = 10.
  // m_command will be 200 (P) + 10 (I) = 210 -> capped at 10.
  float cmd = pid.command(0.0f, 1000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, 10.0f, cmd);

  // Second call: m_command is 10.
  // saturation threshold = 10 * 0.99 - 10 * 0.01 = 9.8.
  // m_command (10) > 9.8 and error (10) > 0 => command_is_saturated() is true.
  // I should NOT update. I remains 10.
  cmd = pid.command(0.0f, 2000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, 10.0f, cmd);

  // Third call: check if I is still 10.
  // error = 10 - 9.8 = 0.2.
  // P = 0.2 * 20 = 4.
  // cmd = 4 (P) + 10 (I) = 14 -> capped at 10.
  // To verify anti-windup, I need error small enough that P + I < 10.
  // error = 10 - 10 = 0.
  // P = 0.
  // cmd = 0 (P) + 10 (I) = 10.
  cmd = pid.command(10.0f, 3000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, 10.0f, cmd);

  // If anti-windup FAILED, I would be 10 (from call 1) + 10 (from call 2) = 20.
  // We need to decrease I to see it.

  // error = 10 - 15 = -5.
  // command_is_saturated(): error < 0, threshold = 10*0.01 - 10*0.99 = -9.8.
  // m_command (10) is NOT < -9.8. So I will update.
  // I = 10 + (-5) * 1s * 1.0 = 5.
  // P = -5 * 20 = -100.
  // cmd = -100 (P) + 5 (I) = -95 -> capped at -10.
  cmd = pid.command(15.0f, 4000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, -10.0f, cmd);

  // error = 10 - 10 = 0.
  // P = 0.
  // cmd = 0 (P) + 5 (I) = 5.
  cmd = pid.command(10.0f, 5000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, 5.0f, cmd);
}

void test_pid_feedforward() {
  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

  og3::VariableGroup vg("vg");
  og3::VariableGroup cfg_vg("cfg_vg");
  og3::VariableGroup cmd_vg("cmd_vg");
  og3::PID::Gains gains = og3::PID::Gains(1.0f).withFeedforward(5.0f);
  og3::PID pid(gains, vg, cfg_vg, cmd_vg);

  pid.setTarget(10.0f);
  pid.initialize();

  float cmd = pid.command(10.0f, 1000);
  TEST_ASSERT_FLOAT_WITHIN(1e-5, 5.0f, cmd);
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_pid_p);
  RUN_TEST(test_pid_i);
  RUN_TEST(test_pid_d);
  RUN_TEST(test_pid_saturation);
  RUN_TEST(test_pid_feedforward);
  return UNITY_END();
}

int main() { return runUnityTests(); }

void setup() {}
void loop() {}
void app_main() { runUnityTests(); }
