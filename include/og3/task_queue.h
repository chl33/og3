// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <cstdio>
#include <functional>
#include <vector>

#include "og3/util.h"

namespace og3 {

/**
 * @brief Represents a callback task scheduled for a specific time.
 */
struct TimedThunk {
  unsigned long msec = 0;  ///< Time in milliseconds when the task should run.
  Thunk thunk = nullptr;   ///< The callback function to execute.
  unsigned id = 0;         ///< Unique identifier for the task (0 for none).
};

/**
 * @brief Manages a priority queue of timed callbacks.
 *
 * TaskQueue stores callbacks to be executed at specific times. Tasks can
 * optionally have a unique ID; if a new task is added with an existing ID,
 * it replaces the previous one.
 */
class TaskQueue {
 public:
  /** @brief Constructs a TaskQueue with a fixed capacity. */
  explicit TaskQueue(std::size_t capacity) : m_thunks(capacity) {}
  TaskQueue(const TaskQueue&) = delete;

  /** @return The maximum number of callbacks that can be queued. */
  std::size_t capacity() const { return m_thunks.size(); }

  /** @return The number of currently scheduled callbacks. */
  std::size_t size() const { return m_size; }
  /** @return true if no tasks are scheduled. */
  bool empty() const { return size() == 0; }
  /** @return true if the queue is at capacity. */
  bool full() const { return size() == capacity(); }

  /** @return Reference to a task by index. */
  TimedThunk& thunk(std::size_t idx) { return m_thunks[index(idx)]; }
  /** @return Constant reference to a task by index. */
  const TimedThunk& thunk(std::size_t idx) const { return m_thunks[index(idx)]; }
  /** @return Constant reference to the next task to run. */
  const TimedThunk& first() const { return thunk(0); }
  /** @return Reference to the next task to run. */
  TimedThunk& first() { return thunk(0); }
  /** @return Constant reference to the task scheduled furthest in the future. */
  const TimedThunk& last() const { return thunk(size() - 1); }
  /** @return Timestamp of the next task, or 0 if empty. */
  unsigned long nextMsec() const { return empty() ? 0 : first().msec; }

  /**
   * @brief Schedules a task.
   * @param msec Target system time (millis).
   * @param t The callback.
   * @param id Optional task ID.
   * @return true if successfully inserted.
   */
  bool insert(unsigned long msec, const Thunk& t, unsigned id = 0);

  /**
   * @brief Schedules a task, replacing any existing task with the same ID.
   * @param msec Target system time (millis).
   * @param t The callback.
   * @param id Task ID.
   */
  void insertReplace(unsigned long msec, const Thunk& t, unsigned id = 0);

  /**
   * @brief Removes a task by ID.
   * @param id The ID to remove.
   * @return true if a task was found and removed.
   */
  bool remove(unsigned id);

  /**
   * @brief Executes the next scheduled task if its time has arrived.
   * @return true if a task was executed.
   */
  bool runNext();

  /** @brief Removes the next scheduled task without executing it. */
  void popFirst();

  /** @brief Debug utility to print the queue state. */
  void show() const;

  /** @return The current internal starting index. */
  std::size_t start_idx() const { return m_start_idx; }

  /** @return Generates a new unique task ID. */
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
