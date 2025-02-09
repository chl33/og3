// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/task_queue.h"

#include <array>
#include <queue>
#include <random>

#include "unity.h"

void setUp() {}

void tearDown() {}

void test_eq1() {
  og3::TaskQueue eq(1);
  TEST_ASSERT_EQUAL(1, eq.capacity());
  TEST_ASSERT_EQUAL(0, eq.size());
  TEST_ASSERT_TRUE(eq.empty());

  // Add a single task -> [10]
  bool t = false;
  auto fn = [&t]() { t = true; };
  TEST_ASSERT_TRUE(eq.insert(10, fn));
  // eq.show();
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_FALSE(eq.empty());
  TEST_ASSERT_EQUAL(10, eq.first().msec);
  TEST_ASSERT_EQUAL(10, eq.nextMsec());

  // Try to add a second task -- it shouldn't work.
  TEST_ASSERT_FALSE(eq.insert(20, fn));
  // eq.show();
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_FALSE(eq.empty());

  // Run the task.  This will clear the queue.
  TEST_ASSERT_TRUE(eq.runNext());
  // eq.show();
  TEST_ASSERT_TRUE(t);
  TEST_ASSERT_TRUE(eq.empty());

  // Try to add a third task -- it should work. -> [30]
  TEST_ASSERT_TRUE(eq.insert(30, fn));
  // eq.show();
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_EQUAL(30, eq.first().msec);
  TEST_ASSERT_EQUAL(30, eq.nextMsec());
  TEST_ASSERT_FALSE(eq.empty());

  // Try to overwrite with a fourth task after current.  -> [30]
  eq.insertReplace(40, fn);
  // eq.show();
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_EQUAL(30, eq.first().msec);
  TEST_ASSERT_EQUAL(30, eq.nextMsec());

  // Run the task.  This will clear the queue.
  t = false;
  TEST_ASSERT_TRUE(eq.runNext());
  // eq.show();
  TEST_ASSERT_TRUE(t);
  TEST_ASSERT_TRUE(eq.empty());
}

void test_eq2() {
  og3::TaskQueue eq(2);
  TEST_ASSERT_EQUAL(2, eq.capacity());
  TEST_ASSERT_EQUAL(0, eq.size());
  TEST_ASSERT_TRUE(eq.empty());
  // eq.show();

  // Add a single task. -> [(10,1)]
  int ti = -1;
  auto fn = [&ti](int i) { ti = i; };
  TEST_ASSERT_TRUE(eq.insert(10, std::bind(fn, 1)));
  // eq.show();
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_FALSE(eq.empty());
  TEST_ASSERT_EQUAL(10, eq.first().msec);
  TEST_ASSERT_EQUAL(10, eq.nextMsec());

  // Add a second task. -> [(10,1), (20,2)]
  TEST_ASSERT_TRUE(eq.insert(20, std::bind(fn, 2)));
  // eq.show();
  TEST_ASSERT_EQUAL(2, eq.size());
  TEST_ASSERT_EQUAL(10, eq.nextMsec());

  // Try to add a third task -- it should fail. -> [(10,1), (20,2)]
  TEST_ASSERT_FALSE(eq.insert(30, std::bind(fn, 3)));
  // eq.show();

  // Add a fourth task, overriding the second. -> [(5, 4), (10,1)]
  eq.insertReplace(5, std::bind(fn, 4));
  // eq.show();
  TEST_ASSERT_EQUAL(2, eq.size());
  TEST_ASSERT_EQUAL(5, eq.first().msec);

  // Add a fourth-a task, not overriding the second. -> [(5, 4), (10,1)]
  eq.insertReplace(15, std::bind(fn, 4));
  // eq.show();
  TEST_ASSERT_EQUAL(2, eq.size());
  TEST_ASSERT_EQUAL(5, eq.first().msec);

  // Run the task. -> [(10, 1)]
  TEST_ASSERT_TRUE(eq.runNext());
  // eq.show();
  TEST_ASSERT_EQUAL(4, ti);
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_EQUAL(10, eq.first().msec);

  // Add a 5th task. -> [(10, 1), (50, 5)]
  TEST_ASSERT_TRUE(eq.insert(50, std::bind(fn, 5)));
  // eq.show();
  TEST_ASSERT_EQUAL(2, eq.size());
  TEST_ASSERT_EQUAL(10, eq.first().msec);

  // Run the task. -> [(50, 5)]
  TEST_ASSERT_TRUE(eq.runNext());
  // eq.show();
  TEST_ASSERT_EQUAL(1, ti);
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_EQUAL(50, eq.first().msec);

  // Run the task. -> []
  TEST_ASSERT_TRUE(eq.runNext());
  // eq.show();
  TEST_ASSERT_EQUAL(5, ti);
  TEST_ASSERT_EQUAL(0, eq.size());
}

void test_eq3() {
  og3::TaskQueue eq(8);
  TEST_ASSERT_EQUAL(8, eq.capacity());
  TEST_ASSERT_EQUAL(0, eq.size());
  TEST_ASSERT_TRUE(eq.empty());

  int ti = -1;
  auto fn = [&ti](int i) { ti = i; };
  constexpr unsigned kId1 = 100;
  constexpr unsigned kId2 = 101;

  // -> [(50, 1, 100)]
  TEST_ASSERT_TRUE(eq.insert(
      50, [&fn]() { fn(1); }, kId1));
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_EQUAL(50, eq.first().msec);
  TEST_ASSERT_EQUAL(kId1, eq.first().id);

  // -> [(50, 1, 100)]
  TEST_ASSERT_TRUE(eq.insert(
      50, [&fn]() { fn(1); }, kId1));
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_EQUAL(50, eq.first().msec);
  TEST_ASSERT_EQUAL(kId1, eq.first().id);

  // -> [(10, 2, 101), (50, 1, 100)]
  TEST_ASSERT_TRUE(eq.insert(
      10, [fn]() { fn(2); }, kId2));
  TEST_ASSERT_EQUAL(2, eq.size());
  TEST_ASSERT_EQUAL(10, eq.first().msec);
  TEST_ASSERT_EQUAL(kId2, eq.first().id);

  // -> [(50, 1, 100), (60, 3, 101)]
  TEST_ASSERT_TRUE(eq.insert(
      60, [fn]() { fn(3); }, kId2));
  TEST_ASSERT_EQUAL(2, eq.size());
  TEST_ASSERT_EQUAL(50, eq.first().msec);
  TEST_ASSERT_EQUAL(kId1, eq.first().id);

  // -> [(60, 3, 101)]
  TEST_ASSERT_TRUE(eq.runNext());
  // eq.show();
  TEST_ASSERT_EQUAL(1, ti);
  TEST_ASSERT_EQUAL(1, eq.size());
  TEST_ASSERT_EQUAL(60, eq.first().msec);
}

void test_eq4() {
  og3::TaskQueue eq(32);

  std::mt19937 gen32;  // Standard mersenne_twister_engine seeded with rd()

  int ti = -1;
  auto fn = [&ti](int i) { ti = i; };

  std::array<unsigned, 20> counts;
  unsigned filled = 0;

  for (unsigned i = 0; i < 200 && filled < 20; i++) {
    const unsigned val = gen32() % 20;
    if (counts[val] == 0) {
      filled += 1;
    }
    counts[val] += 1;

    TEST_ASSERT_TRUE(eq.insert(
        val * 100, [&fn, val]() { fn(val); }, val + 10));
  }

  for (unsigned i = 0; i < 20; i++) {
    TEST_ASSERT_GREATER_THAN_UINT(0, counts[i]);
  }

  for (unsigned i = 0; i < 20; i++) {
    TEST_ASSERT_EQUAL(20 - i, eq.size());

    const unsigned msec = eq.first().msec;
    const unsigned id = eq.first().id;

    TEST_ASSERT_TRUE(eq.runNext());
    TEST_ASSERT_EQUAL(i, ti);
    TEST_ASSERT_EQUAL(i * 100, msec);
    TEST_ASSERT_EQUAL(i + 10, id);
  }
}

void test_eq20() {
  og3::TaskQueue eq(80);
  using Msec_t = unsigned long;

  std::priority_queue<Msec_t, std::vector<Msec_t>, std::greater<Msec_t>> queue;

  std::mt19937 gen32;  // Standard mersenne_twister_engine seeded with rd()
  constexpr Msec_t kMaxPriority = 200;
  std::uniform_int_distribution<Msec_t> distribution(0, kMaxPriority);

  Msec_t ti = 0xFF;
  auto fn = [&ti](Msec_t i) { ti = i; };

  auto getVal = [&ti](const og3::TimedThunk& tt) -> Msec_t {
    tt.thunk();
    return ti;
  };
  // Cause time values to wrap-around.
  auto getTime = [kMaxPriority](Msec_t priority) -> Msec_t {
    constexpr Msec_t kOffset = std::numeric_limits<Msec_t>::max() - kMaxPriority / 2;
    // printf("kOffset: %lu\n", kOffset);
    return static_cast<Msec_t>(priority + kOffset);
  };
  auto addElement = [&fn, &eq, &queue, &getTime](Msec_t priority) {
    // printf("+ %lu(%ld)\n", priority, getTime(priority));
    eq.insertReplace(getTime(priority), std::bind(fn, priority));
    queue.push(priority);
  };
#if 0
  auto show = [&getVal, &eq]() {
    printf("st:%zd [", eq.start_idx());
    for (std::size_t i = 0; i < eq.size(); i++) {
      if (i > 0) {
        printf(", ");
      }
      printf("%ld(%lu)", eq.thunk(i).msec, getVal(eq.thunk(i)));
    }
    printf("]\n");
  };
#endif

  for (Msec_t i = 0; i < 1000; i++) {
    addElement(distribution(gen32));
    // show();
    // Pop the first element a quarter of the time.
    if ((gen32() % 4) == 0) {
      const Msec_t expected_priority = queue.top();
      TEST_ASSERT_EQUAL(expected_priority, getVal(eq.first()));
      queue.pop();
      eq.popFirst();
    }
  }

  TEST_ASSERT_EQUAL(eq.capacity(), eq.size());
  for (Msec_t i = 0; i < eq.size(); i++) {
    TEST_ASSERT_FALSE(queue.empty());
    const Msec_t expected_priority = queue.top();
    TEST_ASSERT_EQUAL(expected_priority, getVal(eq.thunk(i)));
    queue.pop();
  }
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_eq1);
  RUN_TEST(test_eq2);
  RUN_TEST(test_eq3);
  RUN_TEST(test_eq4);
  RUN_TEST(test_eq20);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduion framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
