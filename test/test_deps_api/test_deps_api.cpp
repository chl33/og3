#include <unity.h>

#include "og3/logger.h"
#include "og3/module.h"
#include "og3/module_system.h"

using namespace og3;

class MockModule : public Module {
 public:
  MockModule(const char* name, ModuleSystem* sys) : Module(name, sys) {}

  void setDeps(const std::vector<const char*>& deps) { setDependencies(deps); }
  void setDep(const char* dep) { setDependencies(dep); }
};

class MockLogger : public Logger {
  void log(const char* msg) override {}
};

void test_vector_dependencies() {
  MockLogger logger;
  Logger* log_ptr = &logger;
  ModuleSystem sys(&log_ptr);

  MockModule modA("modA", &sys);
  MockModule modB("modB", &sys);
  MockModule modC("modC", &sys);

  modC.setDeps({"modA", "modB"});

  // Manually resolve to check
  NameToModule n2m;
  n2m["modA"] = &modA;
  n2m["modB"] = &modB;

  sys.setup();  // This calls resolve
}

void test_single_dependency() {
  MockLogger logger;
  Logger* log_ptr = &logger;
  ModuleSystem sys(&log_ptr);

  MockModule modA("modA", &sys);
  MockModule modB("modB", &sys);

  modB.setDep("modA");

  sys.setup();
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_vector_dependencies);
  RUN_TEST(test_single_dependency);
  return UNITY_END();
}
