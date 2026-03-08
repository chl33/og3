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
 * @brief Manages a priority queue of timed callbacks using a fixed-pool doubly-linked list.
 *
 * TaskQueue stores callbacks to be executed at specific times. Tasks can
 * optionally have a unique ID; if a new task is added with an existing ID,
 * it replaces the previous one. This implementation avoids runtime memory
 * allocations by using a pre-allocated pool of nodes.
 */
class TaskQueue {
 public:
  /** @brief Pointer/Index type for the internal pool. */
  using NodeIdx = int16_t;
  static constexpr NodeIdx kInvalidIdx = -1;

  /** @brief Internal node structure for the doubly-linked list. */
  struct Node {
    TimedThunk data;
    NodeIdx prev = kInvalidIdx;
    NodeIdx next = kInvalidIdx;
  };

  /** @brief Constructs a TaskQueue with a fixed capacity. */
  explicit TaskQueue(std::size_t capacity);
  TaskQueue(const TaskQueue&) = delete;

  /** @return The maximum number of callbacks that can be queued. */
  std::size_t capacity() const { return m_pool.size(); }

  /** @return The number of currently scheduled callbacks. */
  std::size_t size() const { return m_size; }
  /** @return true if no tasks are scheduled. */
  bool empty() const { return size() == 0; }
  /** @return true if the queue is at capacity. */
  bool full() const { return size() == capacity(); }

  /** @return Constant reference to the next task to run. */
  const TimedThunk& first() const { return m_pool[m_head].data; }
  /** @return Reference to the next task to run. */
  TimedThunk& first() { return m_pool[m_head].data; }
  /** @return Timestamp of the next task, or 0 if empty. */
  unsigned long nextMsec() const { return empty() ? 0 : first().msec; }

  /**
   * @brief Access a task by logical index (O(N)).
   * This is primarily for test compatibility and debugging.
   * @param idx Logical index from 0 to size() - 1.
   * @return Reference to the TimedThunk.
   */
  TimedThunk& thunk(std::size_t idx);
  const TimedThunk& thunk(std::size_t idx) const;

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

  /** @brief Removes the last scheduled task (the one furthest in the future). */
  void popLast();

  /** @brief Debug utility to print the queue state. */
  void show() const;

  /** @return Generates a new unique task ID. */
  unsigned getId() { return m_first_id++; }

 private:
  NodeIdx allocNode();
  void freeNode(NodeIdx idx);
  void linkBefore(NodeIdx nodeIdx, NodeIdx beforeIdx);
  void linkAfter(NodeIdx nodeIdx, NodeIdx afterIdx);
  void unlink(NodeIdx idx);

  std::vector<Node> m_pool;
  NodeIdx m_head = kInvalidIdx;
  NodeIdx m_tail = kInvalidIdx;
  NodeIdx m_free_head = kInvalidIdx;
  std::size_t m_size = 0;

  unsigned m_first_id = 200;
};

}  // namespace og3
