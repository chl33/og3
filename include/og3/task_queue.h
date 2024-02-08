// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <cstdio>
#include <functional>
#include <limits>
#include <vector>

#include "og3/util.h"

namespace og3 {

struct TimedThunk {
  unsigned long msec = 0;
  Thunk thunk = nullptr;
  unsigned id = 0;
};

// Return true if t1 is before t2, allowing that t1 may have wrapped around.
inline bool isBefore(unsigned long t1, unsigned long t2) {
  const unsigned long d = t2 - t1;
  return d < std::numeric_limits<unsigned long>::max() / 2;
}

// TaskQueue stores callbacks to be called at particular times.
// Tasks can optionally be stored with a numeric ID.
// Only one task with a given ID may be in the queue at one time,
//  so queuing an task with an ID will replace any other task
//  with that ID in the queue.
class TaskQueue {
 public:
  // Capacity is the maximum number number of callbacks which can be queued.
  explicit TaskQueue(std::size_t capacity) : m_thunks(capacity) {}
  TaskQueue(const TaskQueue&) = delete;

  // The maximum number number of callbacks which can be queued.
  std::size_t capacity() const { return m_thunks.size(); }

  // The number of callbacks currently queued.
  std::size_t size() const { return m_size; }
  bool empty() const { return size() == 0; }
  bool full() const { return size() == capacity(); }

  TimedThunk& thunk(std::size_t idx) { return m_thunks[index(idx)]; }
  const TimedThunk& thunk(std::size_t idx) const { return m_thunks[index(idx)]; }
  const TimedThunk& first() const { return thunk(0); }
  TimedThunk& first() { return thunk(0); }
  const TimedThunk& last() const { return thunk(size() - 1); }
  unsigned long nextMsec() const { return empty() ? 0 : first().msec; }

  bool insert(unsigned long msec, const Thunk& t, unsigned id = 0);
  void insertReplace(unsigned long msec, const Thunk& t, unsigned id = 0);
  bool remove(unsigned id);
  bool runNext();
  void popFirst();
  void show() const;
  std::size_t start_idx() const { return m_start_idx; }

  unsigned getId() { return m_first_id++; }

 protected:
  std::size_t index(std::size_t idx) const { return (m_start_idx + idx) % capacity(); }
  void shift(std::size_t num);
  void lshift(std::size_t num);
  void incrementStart() { m_start_idx = (m_start_idx + 1) % capacity(); }
  void decrementStart() { m_start_idx = (m_start_idx + (capacity() - 1)) % capacity(); }

 private:
  std::vector<TimedThunk> m_thunks;
  std::size_t m_start_idx = 0;
  std::size_t m_size = 0;

  unsigned m_first_id = 200;
};

}  // namespace og3
