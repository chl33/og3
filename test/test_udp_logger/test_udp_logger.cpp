// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <ArduinoFake.h>
#include <unity.h>

#include "og3/app.h"
#include "og3/udp_logger.h"
#include "og3/wifi_manager.h"

using namespace og3;
using namespace fakeit;

class TestWifiManager : public WifiManager {
 public:
  TestWifiManager(const char* board_name, Tasks* tasks, const Options& options)
      : WifiManager(board_name, tasks, options) {}

  void triggerDisconnect() { this->onDisconnect(); }
  void triggerConnect() { this->onConnect(); }
};

void test_udp_logger_buffering_and_flush() {
  App::Options app_opts;
  App app(app_opts);
  WifiManager::Options wifi_opts;

  When(Method(ArduinoFake(), millis)).AlwaysReturn(1000);

  TestWifiManager wifi("test-board", &app.tasks(), wifi_opts);
  UdpLogger logger(&wifi, IPAddress(192, 168, 1, 100), 5005);

  TEST_ASSERT_FALSE(logger.is_ready());
  TEST_ASSERT_EQUAL(0, logger.buffered_count());
  TEST_ASSERT_EQUAL(0, logger.dropped_count());

  // Log messages while offline
  logger.log("boot message 1");
  logger.log("boot message 2");
  TEST_ASSERT_EQUAL(2, logger.buffered_count());
  TEST_ASSERT_EQUAL(0, logger.dropped_count());

  // Connect to WiFi -> should trigger flush
  wifi.triggerConnect();
  TEST_ASSERT_TRUE(logger.is_ready());
  TEST_ASSERT_EQUAL(0, logger.buffered_count());

  // While connected, logging does not buffer
  logger.log("online message");
  TEST_ASSERT_EQUAL(0, logger.buffered_count());

  // Disconnect -> buffering resumes
  wifi.triggerDisconnect();
  TEST_ASSERT_FALSE(logger.is_ready());
  logger.log("offline message 1");
  TEST_ASSERT_EQUAL(1, logger.buffered_count());
}

void test_udp_logger_overflow() {
  App::Options app_opts;
  App app(app_opts);
  WifiManager::Options wifi_opts;

  When(Method(ArduinoFake(), millis)).AlwaysReturn(2000);

  TestWifiManager wifi("test-board", &app.tasks(), wifi_opts);
  UdpLogger logger(&wifi, IPAddress(192, 168, 1, 100), 5005);

  // Fill up beyond default capacity (32)
  for (size_t i = 0; i < UdpLogger::kDefaultCapacity + 10; i++) {
    char msg[32];
    snprintf(msg, sizeof(msg), "msg %u", static_cast<unsigned>(i));
    logger.log(msg);
  }

  TEST_ASSERT_EQUAL(UdpLogger::kDefaultCapacity, logger.buffered_count());
  TEST_ASSERT_EQUAL(10, logger.dropped_count());

  // Reconnect -> flushes all and drops count resets upon flush
  wifi.triggerConnect();
  TEST_ASSERT_EQUAL(0, logger.buffered_count());
  TEST_ASSERT_EQUAL(0, logger.dropped_count());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_udp_logger_buffering_and_flush);
  RUN_TEST(test_udp_logger_overflow);
  return UNITY_END();
}
