// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/task_queue.h"

namespace og3 {

// The Tasks module supports running callbacks at scheduled times.
// It uses an TaskQueue to keep a prioritized list of tasks to run.
class Tasks : public Module {
 public:
  static const char* kName;

  static Tasks* get(const NameToModule& n2m) { return GetModule<Tasks>(n2m, kName); }

  Tasks(std::size_t capacity, ModuleSystem* module_system);

  void runAt(unsigned long msec, const Thunk& thunk, unsigned id = 0);
  void runIn(unsigned long msec, const Thunk& thunk, unsigned id = 0);
  unsigned getId() { return m_queue.getId(); }

  std::size_t capacity() const { return m_queue.capacity(); }
  std::size_t size() const { return m_queue.size(); }

  const TaskQueue& queue() const { return m_queue; }

  int loop();

 private:
  bool getThunk(unsigned long now, Thunk* t);

  TaskQueue m_queue;
};

// The TaskIdScheduler assists in scheduling tasks with a given id.
// Queuing an task with a given id will replace any task already queueed with the same id.
class TaskIdScheduler {
 public:
  // You can set tasks = nullptr if you set it later on with setTasks().
  explicit TaskIdScheduler(Tasks* tasks) : m_tasks(tasks), m_id(tasks ? tasks->getId() : 0) {}

  void setTasks(Tasks* tasks) {
    if (m_tasks) {
      return;
    }
    m_tasks = tasks;
    m_id = tasks->getId();
  }
  void runAt(unsigned long msec, const Thunk& thunk) {
    if (m_tasks) {
      m_tasks->runAt(msec, thunk, m_id);
    }
  }
  void runIn(unsigned long msec, const Thunk& thunk) {
    if (m_tasks) {
      m_tasks->runIn(msec, thunk, m_id);
    }
  }

  const Tasks* tasks() const { return m_tasks; }

 private:
  Tasks* m_tasks;
  unsigned m_id;
};

// The TaskScheduler assists in scheduling tasks with a given id.
// Queuing an task with a given id will replace any task already queueed with the same id.
class TaskScheduler {
 public:
  // You can set tasks = nullptr if you set it later on with setTasks().
  TaskScheduler(const Thunk& thunk, Tasks* tasks) : m_thunk(thunk), m_scheduler(tasks) {}

  void setTasks(Tasks* tasks) { m_scheduler.setTasks(tasks); }
  void runAt(unsigned long msec) { m_scheduler.runAt(msec, m_thunk); }
  void runIn(unsigned long msec) { m_scheduler.runIn(msec, m_thunk); }
  const Tasks* tasks() const { return m_scheduler.tasks(); }

 private:
  const Thunk m_thunk;
  TaskIdScheduler m_scheduler;
};

// PeriodicTaskScheduler is like TaskIdQueue but automatically reschedules
//  the task each time it is run.
class PeriodicTaskScheduler {
 public:
  // You can set tasks = nullptr if you set it later on with setTasks().
  PeriodicTaskScheduler(unsigned msec_initial, unsigned period_msec, const Thunk& thunk,
                        Tasks* tasks);

  void setTasks(Tasks* tasks_);

  // Reschedule the time of the next task, shifing all following tasks.
  void runAt(unsigned long msec);
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
