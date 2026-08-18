// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

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

  void triggerStateChange(State s) { this->changeState(s); }

  unsigned long getRetryDelay() const { return m_retry_delay_ms; }
};

void test_state_transitions() {
  App::Options app_opts;
  App app(app_opts);
  WifiManager::Options wifi_opts;

  When(Method(ArduinoFake(), millis)).AlwaysReturn(10000);

  TestWifiManager wifi("test-board", &app.tasks(), wifi_opts);
  TEST_ASSERT_EQUAL(WifiManager::State::kIdle, wifi.state());
  TEST_ASSERT_EQUAL_STRING("idle", wifi.stateString());
  TEST_ASSERT_FALSE(wifi.apMode());
  TEST_ASSERT_FALSE(wifi.wasConnected());

  // Connect
  wifi.triggerConnect();
  TEST_ASSERT_EQUAL(WifiManager::State::kConnectedSta, wifi.state());
  TEST_ASSERT_EQUAL_STRING("connected", wifi.stateString());
  TEST_ASSERT_TRUE(wifi.wasConnected());

  // Disconnect -> should transition to WaitingReconnectSta
  wifi.triggerDisconnect();
  TEST_ASSERT_EQUAL(WifiManager::State::kWaitingReconnectSta, wifi.state());
  TEST_ASSERT_EQUAL_STRING("reconnecting", wifi.stateString());

  // Switch to SoftAP
  wifi.triggerStateChange(WifiManager::State::kSoftAp);
  TEST_ASSERT_EQUAL(WifiManager::State::kSoftAp, wifi.state());
  TEST_ASSERT_EQUAL_STRING("soft-ap", wifi.stateString());
  TEST_ASSERT_TRUE(wifi.apMode());
}

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
  RUN_TEST(test_state_transitions);
  RUN_TEST(test_backoff_increase);
  RUN_TEST(test_backoff_reset);
  return UNITY_END();
}
