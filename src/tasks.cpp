// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/tasks.h"

#include <Arduino.h>
#ifndef NATIVE
#else
#endif

#include "og3/logger.h"
#include "og3/module.h"
#include "og3/task_queue.h"

namespace og3 {

const char* Tasks::kName = "tasks";

Thunk Tasks::s_run_next = nullptr;

Tasks::Tasks(std::size_t capacity, ModuleSystem* module_system)
    : Module(kName, module_system), m_queue(capacity) {
  add_update_fn([this]() { loop(); });
}

void Tasks::runAt(unsigned long msec, const Thunk& thunk, unsigned id) {
  bool ok = false;
  noInterrupts();
  ok = m_queue.insert(msec, thunk, id);
  interrupts();
  if (!ok) {
    log()->logf("Failed to schedule task callback (full!) id=%u", id);
  }
}

void Tasks::runIn(unsigned long msec, const Thunk& thunk, unsigned id) {
  runAt(millis() + msec, thunk, id);
}

int Tasks::loop() {
  const auto now = millis();
  Thunk thunk;
  int count = 0;
  while (getThunk(now, &thunk)) {
    thunk();
    count += 1;
  }
  return count;
}

bool Tasks::getThunk(unsigned long now, Thunk* t) {
  bool ret = false;
  noInterrupts();
  if (s_run_next) {
    *t = s_run_next;
    s_run_next = nullptr;
    ret = true;
  } else if (!m_queue.empty() && isBefore(m_queue.nextMsec(), now)) {
    *t = m_queue.first().thunk;
    m_queue.popFirst();
    ret = true;
  }
  interrupts();
  return ret;
}

PeriodicTaskScheduler::PeriodicTaskScheduler(unsigned msec_initial, unsigned period_msec,
                                             const Thunk& thunk, Tasks* tasks)
    : m_scheduler(tasks), m_thunk(thunk), m_initial_msec(msec_initial), m_period_msec(period_msec) {
  if (tasks) {
    start(millis() + msec_initial);
  }
}

void PeriodicTaskScheduler::setTasks(Tasks* tasks_) {
  if (m_scheduler.tasks()) {
    return;  // Tasks was already set.
  }
  m_scheduler.setTasks(tasks_);
  start(millis() + m_initial_msec);
}

// Reschedule the time of the next task, shifing all following tasks.
void PeriodicTaskScheduler::runAt(unsigned long msec) {
  if (m_scheduler.tasks()) {
    start(msec);
  }
}
void PeriodicTaskScheduler::runIn(unsigned long msec) {
  if (m_scheduler.tasks()) {
    start(millis() + msec);
  }
}

void PeriodicTaskScheduler::start(unsigned long msec) {
  m_prev_msec = millis();
  m_scheduler.runAt(msec, [this]() { cycle(); });
}
void PeriodicTaskScheduler::cycle() {
  // Run the callback.
  m_thunk();
  if (m_period_msec == 0) {
    return;
  }
  // Schedule the next callback for the next multiple of period that is after the current time.
  const unsigned long now_msec = millis();
  unsigned long next_msec = m_prev_msec + m_period_msec;
  while (isBefore(next_msec, now_msec)) {  // wrap-around-friendly comparison
    next_msec += m_period_msec;
  }
  m_scheduler.runAt(next_msec, [this]() { cycle(); });
  m_prev_msec = next_msec;
}

}  // namespace og3
