// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/relay.h"

#include "og3/logger.h"

namespace og3 {

Relay::Relay(const char* name_, Tasks* tasks, uint8_t pin_, const char* description, bool publish,
             VariableGroup& vg, OnLevel on_level)
    : DOut(name_, false, tasks->module_system(), pin_, description, publish, vg),
      m_on_level(on_level),
      m_tasks(tasks) {
  turnOff();
}

bool Relay::turnOn(int on_msec, const std::function<void()>& off_fn) {
  const bool was_on = isOn();
  set(m_on_level == OnLevel::kHigh ? true : false);
  m_last_on_msec = millis();
  if (!was_on) {
    log()->logf("%s -> on", name());
  }
  if (on_msec > 0) {
    turnOffIn(on_msec, off_fn);
  }
  return !was_on;
}

void Relay::turnOffIn(int msec, const std::function<void()>& fn) {
  m_off_callback = fn;
  m_tasks->runIn(msec, [this]() { turnOff(); });
}

void Relay::turnOff() {
  const bool was_on = isOn();
  set(m_on_level == OnLevel::kHigh ? false : true);
  if (m_off_callback) {
    m_off_callback();
    m_off_callback = nullptr;
  }
  if (was_on) {
    m_last_on_msec = millis();
    log()->logf("%s -> off", name());
  }
}

}  // namespace og3
