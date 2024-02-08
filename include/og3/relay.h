// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/dout.h"
#include "og3/tasks.h"

namespace og3 {

// Relay is a wrapper around a digital output for controling a relay.
class Relay : public DOut {
 public:
  enum class OnLevel {
    kHigh = 0,  // Relay is on when output pin is high.
    kLow = 1,   // Relay is on when output pin is low.
  };

  Relay(const char* name_, Tasks* tasks, uint8_t pin_, const char* description, bool publish,
        VariableGroup* vg, OnLevel on_level = OnLevel::kHigh);

  bool turnOn(int on_msec = 0, const std::function<void()>& off_fn = nullptr);
  void turnOffIn(int msec, const std::function<void()>& fn = nullptr);
  void turnOff();
  bool isOn() const { return isHigh() == (m_on_level == OnLevel::kHigh); }
  unsigned long lastOnMsec() const { return isOn() ? millis() : m_last_on_msec; }

 private:
  const OnLevel m_on_level;
  unsigned long m_last_on_msec = 0;
  std::function<void()> m_off_callback;
  Tasks* m_tasks;
};

}  // namespace og3
