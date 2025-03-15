// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

// #include <ArduinoFake.h>

#include "og3/app.h"
#include "og3/kernel_filter.h"
#include "og3/units.h"
#include "unity.h"

namespace {
class StdoutLogger : public og3::Logger {
  void log(const char* msg) final { printf("%s\n", msg); }
};
}  // namespace

void setUp() {}

void tearDown() {}

void test_filter() {
  og3::App app({});
  og3::VariableGroup vg("test");
  constexpr unsigned kSize = 8;
  og3::KernelFilter::Options filter_options({
      .name = "filtered_value",
      .units = og3::units::kPercentage,
      .description = "filtered value",
      .var_flags = 0,
      .sigma = 10.0,
      .decimals = 1,
      .size = kSize,
  });
  og3::KernelFilter filter(filter_options, &app.module_system(), vg);
  {
    og3::KernelFilter::State<1> wrong_size;
    TEST_ASSERT_FALSE(filter.saveState(wrong_size));
  }
  og3::KernelFilter::State<kSize> state;
  for (unsigned i = 0; i < kSize * 2; i++) {
    const float fi = i;
    filter.addSample(fi, fi);
    const float val_i = filter.computeValue(fi);
    TEST_ASSERT_TRUE(filter.saveState(state));
    {
      og3::KernelFilter filter_copy(filter_options, &app.module_system(), vg);
      TEST_ASSERT_TRUE(filter_copy.restoreState(state));
      const float copy_val_i = filter_copy.computeValue(fi);
      TEST_ASSERT_EQUAL(val_i, copy_val_i);
    }
  }
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_filter);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduino framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
