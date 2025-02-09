// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <queue>
#include <random>

#include "og3/ring_buffer.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

void test_rq1() {
  og3::RingQueue<int, 4> rq;
  TEST_ASSERT_TRUE(rq.empty());
  TEST_ASSERT_EQUAL(4, rq.capacity());
  TEST_ASSERT_EQUAL(0, rq.size());
  rq.pushFront(1);  // 1
  TEST_ASSERT_EQUAL(1, rq.size());
  TEST_ASSERT_EQUAL(1, rq.front());
  TEST_ASSERT_EQUAL(1, rq.back());
  rq.pushBack(2);  // 1,2
  TEST_ASSERT_EQUAL(2, rq.size());
  TEST_ASSERT_EQUAL(1, rq.front());
  TEST_ASSERT_EQUAL(2, rq.back());
  rq.pushBack(3);  // 1,2,3
  TEST_ASSERT_EQUAL(3, rq.size());
  TEST_ASSERT_EQUAL(1, rq.front());
  TEST_ASSERT_EQUAL(3, rq.back());
  rq.pushFront(0);  // 0,1,2,3
  TEST_ASSERT_EQUAL(4, rq.size());
  TEST_ASSERT_EQUAL(0, rq.front());
  TEST_ASSERT_EQUAL(3, rq.back());
  rq.pushFront(-1);  // -1,0,1,2
  TEST_ASSERT_EQUAL(4, rq.size());
  TEST_ASSERT_EQUAL(-1, rq.front());
  TEST_ASSERT_EQUAL(2, rq.back());
  rq.pushBack(4);  // 0,1,2,4
  TEST_ASSERT_EQUAL(4, rq.size());
  TEST_ASSERT_EQUAL(0, rq.front());
  TEST_ASSERT_EQUAL(4, rq.back());
  rq.popFront();  // 1,2,4
  TEST_ASSERT_EQUAL(3, rq.size());
  TEST_ASSERT_EQUAL(1, rq.front());
  TEST_ASSERT_EQUAL(4, rq.back());
  rq.popBack();  // 1,2,
  TEST_ASSERT_EQUAL(2, rq.size());
  TEST_ASSERT_EQUAL(1, rq.front());
  TEST_ASSERT_EQUAL(2, rq.back());
  rq.popFront();  // 2
  TEST_ASSERT_EQUAL(1, rq.size());
  TEST_ASSERT_EQUAL(2, rq.front());
  TEST_ASSERT_EQUAL(2, rq.back());
  rq.popBack();
  TEST_ASSERT_EQUAL(0, rq.size());
  rq.popBack();
  TEST_ASSERT_EQUAL(0, rq.size());
  rq.popFront();
  TEST_ASSERT_EQUAL(0, rq.size());
}

void check_ring(const og3::SortedRing<unsigned, 32>& sr) {
  for (size_t i = 0; i < sr.size() - 1; i++) {
    TEST_ASSERT_GREATER_OR_EQUAL_UINT(sr[i], sr.element(i + 1));
  }
}

void test_sr1() {
  og3::SortedRing<unsigned, 32> sr;

  TEST_ASSERT_EQUAL(32, sr.capacity());
  TEST_ASSERT_EQUAL(0, sr.size());

  std::mt19937 gen32;  // Standard mersenne_twister_engine seeded with rd()

  for (unsigned i = 0; i < 200; i++) {
    const auto sz0 = sr.size();
    const unsigned val = gen32() % 1000;
    sr.insert(val);
    const auto sz1 = (sz0 < sr.capacity()) ? sz0 + 1 : sz0;
    TEST_ASSERT_EQUAL(sz1, sr.size());
    check_ring(sr);
    switch (gen32() % 4) {
      case 0: {
        const auto sz2 = (sz1 > 0) ? sr.size() - 1 : 0;
        sr.popFront();
        TEST_ASSERT_EQUAL(sz2, sr.size());
        break;
      }
      case 1: {
        const auto sz2 = (sz1 > 0) ? sr.size() - 1 : 0;
        sr.popBack();
        TEST_ASSERT_EQUAL(sz2, sr.size());
        break;
      }
      default:
        break;
    }
  }
}

void test_sr2() {
  og3::SortedRing<unsigned, 8> sr;

  TEST_ASSERT_EQUAL(8, sr.capacity());
  TEST_ASSERT_EQUAL(0, sr.size());

  for (unsigned i = 0; i < 8; i++) {
    sr.insert(i);
  }
  TEST_ASSERT_EQUAL(0, sr.front());  // 0,1,2,3,4,5,6,7
  TEST_ASSERT_EQUAL(7, sr.back());
  TEST_ASSERT_EQUAL(8, sr.size());
  TEST_ASSERT_FALSE(sr.remove([](unsigned val) { return val == 100; }));
  TEST_ASSERT_EQUAL(8, sr.size());  // 0,1,2,3,4,5,6,7
  TEST_ASSERT_TRUE(sr.remove([](unsigned val) { return val == 0; }));
  TEST_ASSERT_EQUAL(7, sr.size());  // 1,2,3,4,5,6,7
  TEST_ASSERT_EQUAL(1, sr.front());
  TEST_ASSERT_FALSE(sr.remove_index(7));
  TEST_ASSERT_TRUE(sr.remove([](unsigned val) { return val == 7; }));
  TEST_ASSERT_EQUAL(6, sr.back());  // 1,2,3,4,5,6
  TEST_ASSERT_EQUAL(6, sr.size());
  TEST_ASSERT_EQUAL(1, sr[0]);
  TEST_ASSERT_EQUAL(2, sr.element(1));
  TEST_ASSERT_EQUAL(3, sr.element(2));
  TEST_ASSERT_EQUAL(4, sr.element(3));
  TEST_ASSERT_EQUAL(5, sr.element(4));
  TEST_ASSERT_EQUAL(6, sr.element(5));
  TEST_ASSERT_TRUE(sr.remove_index(0));
  TEST_ASSERT_EQUAL(2, sr.front());  // 2,3,4,5,6
  TEST_ASSERT_EQUAL(2, sr.element(0));
  TEST_ASSERT_EQUAL(3, sr.element(1));
  TEST_ASSERT_EQUAL(4, sr.element(2));
  TEST_ASSERT_EQUAL(5, sr.element(3));
  TEST_ASSERT_EQUAL(6, sr.element(4));
  TEST_ASSERT_EQUAL(5, sr.size());
  TEST_ASSERT_TRUE(sr.remove_index(1));
  TEST_ASSERT_EQUAL(2, sr.front());  // 2,4,5,6
  TEST_ASSERT_EQUAL(4, sr.size());
  TEST_ASSERT_EQUAL(2, sr.element(0));
  TEST_ASSERT_EQUAL(4, sr.element(1));
  TEST_ASSERT_EQUAL(5, sr.element(2));
  TEST_ASSERT_EQUAL(6, sr.element(3));
  sr.insert(4);
  sr.insert(4);
  sr.insert(4);  // 2,4,4,4,4,5,6
  TEST_ASSERT_EQUAL(2, sr.front());
  sr.popFront();
  TEST_ASSERT_EQUAL(4, sr.front());  // 4,4,4,4,5,6
  TEST_ASSERT_TRUE(sr.remove([](unsigned val) { return val == 4; }));
  TEST_ASSERT_EQUAL(4, sr.front());  // 4,4,4,5,6
  TEST_ASSERT_TRUE(sr.remove([](unsigned val) { return val == 4; }));
  TEST_ASSERT_EQUAL(4, sr.front());  // 4,4,5,6
  TEST_ASSERT_TRUE(sr.remove([](unsigned val) { return val == 4; }));
  TEST_ASSERT_EQUAL(4, sr.front());  // 4,5,6
  TEST_ASSERT_TRUE(sr.remove([](unsigned val) { return val == 4; }));
  TEST_ASSERT_EQUAL(5, sr.front());  // 5,6
  TEST_ASSERT_FALSE(sr.remove([](unsigned val) { return val == 4; }));
  TEST_ASSERT_EQUAL(2, sr.size());
  TEST_ASSERT_EQUAL(5, sr.front());  // 5,6
  TEST_ASSERT_EQUAL(6, sr.back());
  TEST_ASSERT_TRUE(sr.remove([](unsigned val) { return val == 6; }));
  TEST_ASSERT_EQUAL(1, sr.size());
  TEST_ASSERT_EQUAL(5, sr.front());  // 5
  TEST_ASSERT_EQUAL(5, sr.back());   // 5
  TEST_ASSERT_FALSE(sr.remove_index(1));
  TEST_ASSERT_TRUE(sr.remove_index(0));
  TEST_ASSERT_FALSE(sr.remove_index(0));
  TEST_ASSERT_EQUAL(0, sr.size());
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(test_rq1);
  RUN_TEST(test_sr1);
  RUN_TEST(test_sr2);
  return UNITY_END();
}

// For native platform.
int main() { return runUnityTests(); }

// For arduion framework
void setup() {}
void loop() {}

// For ESP-IDF framework
void app_main() { runUnityTests(); }
