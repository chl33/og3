// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/task_queue.h"

namespace og3 {

bool TaskQueue::insert(unsigned long msec, const Thunk& t, unsigned id) {
  if (size() == capacity()) {
    return false;
  }
  insertReplace(msec, t, id);
  return true;
}

void TaskQueue::insertReplace(unsigned long msec, const Thunk& t, unsigned id) {
  if (capacity() < 1) {
    return;
  }
  remove(id);
  if (size() == 0) {
    // First element in the queue.
    m_start_idx = 0;
    first().msec = msec;
    first().thunk = t;
    first().id = id;
    m_size = 1;
    return;
  }
  // If the first element in the list is not before the new element, just insert the
  //  new element at the front of the list.
  if (!isBefore(first().msec, msec)) {
    decrementStart();
    first().msec = msec;
    first().thunk = t;
    first().id = id;
    m_size = std::min(capacity(), m_size + 1);
    return;
  }
  if (full() && isBefore(last().msec, msec)) {
    // Don't add the task if full and all current tasks are before it.
    return;
  }
  std::size_t num_shift = 0;
  while (num_shift < size()) {
    TimedThunk& tti = thunk(size() - num_shift - 1);
    if (isBefore(tti.msec, msec)) {
      break;
    }
    num_shift += 1;
  }
  shift(num_shift);
  TimedThunk& tti = thunk(size() - num_shift);
  tti.msec = msec;
  tti.thunk = t;
  tti.id = id;
  m_size = std::min(capacity(), m_size + 1);
}

bool TaskQueue::remove(unsigned id) {
  if (id == 0) {
    return false;
  }
  std::size_t num_shift = 0;
  while (num_shift < size()) {
    const unsigned idx = size() - num_shift - 1;
    TimedThunk& tti = thunk(idx);
    if (tti.id == id) {
      if (idx == 0) {
        incrementStart();
      } else {
        lshift(num_shift);
      }
      m_size -= 1;
      if (m_size == 0) {
        m_start_idx = 0;
      }
      return true;
    }
    num_shift += 1;
  }
  return false;
}

bool TaskQueue::runNext() {
  if (empty()) {
    return false;  // No thunks to run.
  }
  Thunk thunk = first().thunk;
  popFirst();  // Displose of the run calback.
  thunk();     // Run the first callback.
  return true;
}
void TaskQueue::popFirst() {
  if (empty()) {
    return;
  }
  incrementStart();
  m_size -= 1;
  if (empty()) {
    m_start_idx = 0;
  }
}

void TaskQueue::show() const {
  printf("st:%zu [", m_start_idx);
  for (std::size_t i = 0; i < size(); i++) {
    if (i > 0) {
      printf(", ");
    }
    printf("%lu (%u)", thunk(i).msec, thunk(i).id);
  }
  printf("]\n");
}

void TaskQueue::shift(std::size_t num) {
  for (std::size_t i = 0; i < num; i++) {
    const std::size_t i_src = size() - i - 1;
    const std::size_t i_dst = i_src + 1;
    if (i_dst < capacity()) {
      thunk(i_dst) = thunk(i_src);
    }
  }
}

void TaskQueue::lshift(std::size_t num) {
  for (std::size_t i = num; i > 0; i--) {
    const std::size_t i_dst = size() - i - 1;
    const std::size_t i_src = i_dst + 1;
    if (i_dst < capacity()) {
      thunk(i_dst) = thunk(i_src);
    }
  }
}

}  // namespace og3
