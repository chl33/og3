#include <ArduinoFake.h>
#include <unity.h>

#include "og3/app.h"
#include "og3/wifi_manager.h"

using namespace og3;
using namespace fakeit;

class TestWifiManager : public WifiManager {
 public:
  TestWifiManager(const char* board_name, Tasks* tasks, const Options& options)
      : WifiManager(board_name, tasks, options) {}

  void triggerDisconnect() { this->onDisconnect(); }

  void triggerConnect() { this->onConnect(); }

  unsigned long getRetryDelay() const { return m_retry_delay_ms; }
};

void test_backoff_increase() {
  App::Options app_opts;
  App app(app_opts);
  WifiManager::Options wifi_opts;

  // Setup mocks
  When(Method(ArduinoFake(), millis)).AlwaysReturn(10000);

  TestWifiManager wifi("test-board", &app.tasks(), wifi_opts);

  // Simulate connected state
  wifi.triggerConnect();
  TEST_ASSERT_EQUAL(WifiManager::kInitialRetryDelayMs, wifi.getRetryDelay());

  // First Disconnect
  wifi.triggerDisconnect();
  TEST_ASSERT_EQUAL(WifiManager::kInitialRetryDelayMs * WifiManager::kBackoffMultiplier,
                    wifi.getRetryDelay());

  // Verify task scheduled
  const auto& queue = app.tasks().queue();
  TEST_ASSERT_FALSE(queue.empty());
  // Check that a task is scheduled at 10000 + 5000
  // We need to iterate or check logic.
  // Since sanityCheck is also scheduled (at 1hr), the queue has 2 items.
  // 5000ms < 1hr. So it should be first.
  TEST_ASSERT_EQUAL(10000 + WifiManager::kInitialRetryDelayMs, queue.first().msec);
}

void test_backoff_reset() {
  App::Options app_opts;
  App app(app_opts);
  WifiManager::Options wifi_opts;

  When(Method(ArduinoFake(), millis)).AlwaysReturn(10000);

  TestWifiManager wifi("test-board", &app.tasks(), wifi_opts);

  wifi.triggerConnect();
  wifi.triggerDisconnect();  // delay = 10s (next)

  // Reconnect
  wifi.triggerConnect();  // delay = 5s
  TEST_ASSERT_EQUAL(WifiManager::kInitialRetryDelayMs, wifi.getRetryDelay());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_backoff_increase);
  RUN_TEST(test_backoff_reset);
  return UNITY_END();
}
