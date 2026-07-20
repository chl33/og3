// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>

#include <cstdio>
#include <string>
#include <vector>

#include "og3/logger.h"
#include "og3/module_system.h"
#include "og3/ntp.h"
#include "unity.h"

using namespace fakeit;

namespace og3 {
namespace {
class TestLogger : public Logger {
 public:
  virtual void log(const char* msg) final {}
};

class DummyWifi : public Module {
 public:
  DummyWifi(ModuleSystem* ms) : Module("wifi", ms) {}
  void addConnectCallback(const std::function<void()>& callback) {}
};
}  // namespace
}  // namespace og3

void setUp() { ArduinoFakeReset(); }

void tearDown() {}

void test_ntp_linkage() {
  og3::TestLogger log;
  og3::Logger* plog = &log;
  og3::ModuleSystem ms(&plog);

  og3::DummyWifi wifi(&ms);
  og3::Ntp ntp(&ms, og3::Ntp::Options());

  TEST_ASSERT_TRUE(ms.link());
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_ntp_linkage);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduion framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
