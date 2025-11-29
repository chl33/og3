// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>

#include <cstdio>
#include <string>
#include <vector>

#include "og3/logger.h"
#include "og3/module.h"
#include "og3/module_system.h"
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

class TestModule : public Module {
 public:
  TestModule(const char* name, const std::vector<const char*> predecessors,
             ModuleSystem* module_system)
      : Module(name, module_system) {
    setDependencies(predecessors);
    add_init_fn([this]() { log()->logf("Init %s", this->name()); });
    if (0 != strcmp("test2", name)) {
      add_start_fn([this]() { log()->logf("Start %s", this->name()); });
    }
    if (0 != strcmp("test1", name)) {
      add_update_fn([this]() { log()->logf("Update %s", this->name()); });
    }
  }

 private:
  const char* m_name;
};

}  // namespace

}  // namespace og3

void setUp() {}

void tearDown() {}

void test1() {
  og3::TestLogger log;
  og3::Logger* plog = &log;
  og3::ModuleSystem depends(&plog);
  og3::TestModule test2("test2", {"test1"}, &depends);
  og3::TestModule test1("test1", {}, &depends);
  og3::TestModule test3("test3", {"test2"}, &depends);
  TEST_ASSERT_TRUE(depends.link());
  depends.init();
  TEST_ASSERT_TRUE(log.check("Init test1\nInit test2\nInit test3\n"));
  depends.start();
  TEST_ASSERT_TRUE(log.check("Start test1\nStart test3\n"));
  depends.update();
  TEST_ASSERT_TRUE(log.check("Update test2\nUpdate test3\n"));
}

void test2() {
  og3::TestLogger log;
  og3::Logger* plog = &log;
  og3::ModuleSystem depends(&plog);
  og3::TestModule test3("test3", {"test2"}, &depends);
  og3::TestModule test2("test2", {"test1"}, &depends);
  og3::TestModule test1("test1", {}, &depends);
  TEST_ASSERT_TRUE(depends.setup());
  TEST_ASSERT_TRUE(
      log.check("Init test1\nInit test2\nInit test3\n"
                "Start test1\nStart test3\n"));
  depends.update();
  TEST_ASSERT_TRUE(log.check("Update test2\nUpdate test3\n"));
}

void test3() {
  og3::TestLogger log;
  og3::Logger* plog = &log;
  og3::ModuleSystem depends(&plog);
  // Circular dependencies.
  og3::TestModule test2("test2", {"test1"}, &depends);
  og3::TestModule test1("test1", {"test3"}, &depends);
  og3::TestModule test3("test3", {"test2"}, &depends);
  TEST_ASSERT_FALSE(depends.link());
  TEST_ASSERT_TRUE(log.check("Dependency loop detected at module 'test2'.\n"));
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test1);
  RUN_TEST(test2);
  RUN_TEST(test3);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduion framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
