// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <stdio.h>
#include <string.h>

#include "og3/text_buffer.h"
#include "unity.h"

bool check(const char* expected, const char* value) {
  if (0 == strcmp(expected, value)) {
    return true;
  }
  printf("Got\n - '%s'\n Expected\n - '%s'\n", value, expected);
  return false;
}

void setUp() {}

void tearDown() {}

void test1() {
  og3::TextBuffer<20> buffer;

  buffer.add("aa");
  TEST_ASSERT_TRUE(check("aa", buffer.text()));
  for (unsigned i = 0; i < 10; i++) {
    buffer.split(0);
    TEST_ASSERT_TRUE(check("aa", buffer.text()));
  }
  buffer.clear();
  TEST_ASSERT_TRUE(check("", buffer.text()));

  buffer.add("aa bb cc dd");
  buffer.split(0);
  TEST_ASSERT_TRUE(check("aa bb cc dd", buffer.text()));

  buffer.set("aa bb cc dd");
  buffer.split(4);
  TEST_ASSERT_TRUE(check("aa\nbb\ncc\ndd", buffer.text()));

  buffer.set("aaaa bbbb");
  buffer.split(4);
  TEST_ASSERT_TRUE(check("aaaa\nbbbb", buffer.text()));

  buffer.set("aaaa cccc");
  buffer.split(6);
  TEST_ASSERT_TRUE(check("aaaa\ncccc", buffer.text()));

  buffer.set("aaaa cccc dd");
  buffer.split(4);
  TEST_ASSERT_TRUE(check("aaaa\ncccc\ndd", buffer.text()));

  buffer.set("aaaa ccc ee ff gg");
  buffer.split(6);
  TEST_ASSERT_TRUE(check("aaaa\nccc ee\nff gg", buffer.text()));

  buffer.set("aaaa\nbb dd ee ff");
  buffer.split(6);
  TEST_ASSERT_TRUE(check("aaaa\nbb dd\nee ff", buffer.text()));

  buffer.set("aaaa bb  ddee ff");
  buffer.split(8);
  TEST_ASSERT_TRUE(check("aaaa bb \nddee ff", buffer.text()));

  buffer.set("aaaa %s", "bbb");
  TEST_ASSERT_TRUE(check("aaaa bbb", buffer.text()));
  buffer.add(" cc %s", "dd");
  TEST_ASSERT_TRUE(check("aaaa bbb cc dd", buffer.text()));
  buffer.add(" ffffff");
  TEST_ASSERT_TRUE(check("aaaa bbb cc dd ffff", buffer.text()));
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test1);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduion framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
