// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/task_queue.h"

namespace og3 {

/**
 * @brief Application module for scheduling and executing timed callbacks.
 *
 * The Tasks module manages a TaskQueue and provides an interface for
 * scheduling one-shot tasks. It integrates with the main application loop
 * to execute tasks whose scheduled time has passed.
 */
class Tasks : public Module {
 public:
  static const char* kName;  ///< @brief "tasks"

  /** @return Pointer to the Tasks module instance. */
  static Tasks* get(const NameToModule& n2m) { return GetModule<Tasks>(n2m, kName); }

  /**
   * @brief Direct way to schedule a task from an interrupt context.
   * @param thunk The callback to run in the next loop.
   */
  static void run_next(const Thunk& thunk) { s_run_next = thunk; }

  /** @brief Constructs a Tasks module with a given capacity. */
  Tasks(std::size_t capacity, ModuleSystem* module_system);

  /** @brief Schedules a task to run at an absolute timestamp. */
  void runAt(unsigned long msec, const Thunk& thunk, unsigned id = 0);
  /** @brief Schedules a task to run after a delay from now. */
  void runIn(unsigned long msec, const Thunk& thunk, unsigned id = 0);

  /** @return Generates a unique ID for scheduling related tasks. */
  unsigned getId() { return m_queue.getId(); }

  /** @return Maximum task capacity. */
  std::size_t capacity() const { return m_queue.capacity(); }
  /** @return Current number of scheduled tasks. */
  std::size_t size() const { return m_queue.size(); }

  /** @return Constant reference to the underlying queue. */
  const TaskQueue& queue() const { return m_queue; }

  /**
   * @brief Executes all due tasks. Should be called in loop().
   * @return Number of tasks executed.
   */
  int loop();

 private:
  bool getThunk(unsigned long now, Thunk* t);

  static Thunk s_run_next;

  TaskQueue m_queue;
};

/**
 * @brief Helper for scheduling tasks with a consistent unique ID.
 *
 * This ensures that subsequent calls to `runAt` or `runIn` will replace
 * the previously scheduled task for this specific scheduler instance.
 */
class TaskIdScheduler {
 public:
  /** @brief Constructs a TaskIdScheduler. */
  explicit TaskIdScheduler(Tasks* tasks) : m_tasks(tasks), m_id(tasks ? tasks->getId() : 0) {}

  /** @brief Late-initialization of the tasks module pointer. */
  void setTasks(Tasks* tasks) {
    if (m_tasks) {
      return;
    }
    m_tasks = tasks;
    m_id = tasks->getId();
  }
  /** @brief Schedules the task at an absolute time. */
  void runAt(unsigned long msec, const Thunk& thunk) {
    if (m_tasks) {
      m_tasks->runAt(msec, thunk, m_id);
    }
  }
  /** @brief Schedules the task after a delay. */
  void runIn(unsigned long msec, const Thunk& thunk) {
    if (m_tasks) {
      m_tasks->runIn(msec, thunk, m_id);
    }
  }

  /** @return Pointer to the associated Tasks module. */
  const Tasks* tasks() const { return m_tasks; }

 private:
  Tasks* m_tasks;
  unsigned m_id;
};

/**
 * @brief Specialized scheduler for a single, fixed callback function.
 */
class TaskScheduler {
 public:
  /** @brief Constructs a TaskScheduler. */
  TaskScheduler(const Thunk& thunk, Tasks* tasks) : m_thunk(thunk), m_scheduler(tasks) {}

  void setTasks(Tasks* tasks) { m_scheduler.setTasks(tasks); }
  /** @brief Schedules the fixed callback at an absolute time. */
  void runAt(unsigned long msec) { m_scheduler.runAt(msec, m_thunk); }
  /** @brief Schedules the fixed callback after a delay. */
  void runIn(unsigned long msec) { m_scheduler.runIn(msec, m_thunk); }
  const Tasks* tasks() const { return m_scheduler.tasks(); }

 private:
  const Thunk m_thunk;
  TaskIdScheduler m_scheduler;
};

/**
 * @brief Automatically reschedules a task to run at fixed intervals.
 */
class PeriodicTaskScheduler {
 public:
  /** @brief Constructs a PeriodicTaskScheduler. */
  PeriodicTaskScheduler(unsigned msec_initial, unsigned period_msec, const Thunk& thunk,
                        Tasks* tasks);

  void setTasks(Tasks* tasks_);

  /** @brief Manually reschedules the next execution time. */
  void runAt(unsigned long msec);
  /** @brief Manually reschedules the next execution after a delay. */
  void runIn(unsigned long msec);

 private:
  void start(unsigned long msec);
  void cycle();

  TaskIdScheduler m_scheduler;
  const Thunk m_thunk;
  const unsigned m_initial_msec;
  const unsigned m_period_msec;
  unsigned m_prev_msec = 0;
};

}  // namespace og3
