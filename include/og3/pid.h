// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/variable.h"

namespace og3 {

// This is a PID controller.
class PID {
 public:
  struct Gains {
    Gains(float p_, float i_ = 0, float d_ = 0) : p(p_), i(i_), d(d_) {}
    Gains& operator=(const Gains&) = default;

    Gains& withIMax(float val) {
      i_max = val;
      return *this;
    }
    Gains& withIMin(float val) {
      i_min = val;
      return *this;
    }
    Gains& withFeedforward(float val) {
      feedforward = val;
      return *this;
    }
    Gains& withCommandMax(float val) {
      command_max = val;
      return *this;
    }
    Gains& withCommandMin(float val) {
      command_min = val;
      return *this;
    }

    float p;                      // The proportional error gain.
    float i;                      // The integral error gain.
    float d;                      // The derivative error gain.
    float i_max = 1.0e10;         // The maximum value for the controller integral.
    float i_min = -1.0e10;        // The minimum value for the controller integral.
    float feedforward = 0.0f;     // The feedforward gain.
    float command_min = 1.0e10;   // The minimum allowed output value.
    float command_max = -1.0e10;  // The maximum allowed output value.
  };

  explicit PID(const Gains& gains, VariableGroup* vg, VariableGroup* cfg_vg);

  void initialize() {
    m_last_msec = millis();
    m_error = 0.0f;
    m_d_error = 0.0f;
    m_command = 0.0f;
    m_p_cmd = 0.0f;
    m_d_cmd = 0.0f;
    m_i_cmd = 0.0f;
  }
  void setTarget(float target, float d_target = 0) {
    m_target = target;
    m_d_target = d_target;
  }
  float command(float measured) { return command(measured, millis()); }
  float command(float measured, unsigned now_msec);
  float command(float measured, float d_measured, unsigned now_msec);

  FloatVariable& p() { return m_p; }
  FloatVariable& i() { return m_i; }
  FloatVariable& d() { return m_d; }
  FloatVariable& i_max() { return m_i_max; }
  FloatVariable& i_min() { return m_i_min; }
  FloatVariable& feedforward() { return m_feedforward; }
  FloatVariable& command_min() { return m_command_min; }
  FloatVariable& command_max() { return m_command_max; }

  FloatVariable& target() { return m_target; }
  FloatVariable& d_target() { return m_d_target; }

 private:
  // Gains
  FloatVariable m_p;
  FloatVariable m_i;
  FloatVariable m_d;
  FloatVariable m_i_max;
  FloatVariable m_i_min;
  FloatVariable m_feedforward;
  FloatVariable m_command_min;
  FloatVariable m_command_max;

  float m_last_msec;
  FloatVariable m_target;
  FloatVariable m_d_target;
  FloatVariable m_error;
  FloatVariable m_d_error;
  FloatVariable m_command;
  FloatVariable m_p_cmd;
  FloatVariable m_d_cmd;
  FloatVariable m_i_cmd;
};

}  // namespace og3
