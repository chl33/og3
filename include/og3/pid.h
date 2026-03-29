// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/variable.h"

namespace og3 {

/**
 * @brief A standard Proportional-Integral-Derivative (PID) controller implementation.
 *
 * This controller computes a command output based on measured error and its
 * time-derivative and integral. It includes features like anti-windup (saturation handling)
 * and feedforward. All parameters are integrated with the Variable system for easy
 * tuning via web or config.
 */
class PID {
 public:
  /** @brief Tuning parameters for the PID controller. */
  struct Gains {
    /** @brief Constructs gains with proportional control by default. */
    Gains(float p_, float i_ = 0, float d_ = 0) : p(p_), i(i_), d(d_) {}
    Gains& operator=(const Gains&) = default;

    /** @brief Sets maximum allowed integral contribution. */
    Gains& withIMax(float val) {
      i_max = val;
      return *this;
    }
    /** @brief Sets minimum allowed integral contribution. */
    Gains& withIMin(float val) {
      i_min = val;
      return *this;
    }
    /** @brief Sets constant feedforward gain. */
    Gains& withFeedforward(float val) {
      feedforward = val;
      return *this;
    }
    /** @brief Sets hard maximum output clamp. */
    Gains& withCommandMax(float val) {
      command_max = val;
      return *this;
    }
    /** @brief Sets hard minimum output clamp. */
    Gains& withCommandMin(float val) {
      command_min = val;
      return *this;
    }

    float p;                      ///< The proportional error gain.
    float i;                      ///< The integral error gain.
    float d;                      ///< The derivative error gain.
    float i_max = 1.0e10;         ///< Maximum integral value.
    float i_min = -1.0e10;        ///< Minimum integral value.
    float feedforward = 0.0f;     ///< The feedforward gain.
    float command_min = -1.0e10;  ///< Hard minimum allowed output.
    float command_max = 1.0e10;   ///< Hard maximum allowed output.
  };

  /** @brief Constructs a PID controller and registers its variables. */
  PID(const Gains& gains, VariableGroup& vg, VariableGroup& cfg_vg, VariableGroup& cmd_vg);

  /** @brief Resets the internal controller state (integral and last time). */
  void initialize() {
    m_last_msec = millis();
    m_error = 0.0f;
    m_d_error = 0.0f;
    m_command = 0.0f;
    m_p_cmd = 0.0f;
    m_d_cmd = 0.0f;
    m_i_cmd = 0.0f;
  }

  /** @brief Sets the desired setpoint and its expected rate of change. */
  void setTarget(float target, float d_target = 0) {
    m_target = target;
    m_d_target = d_target;
  }

  /** @brief Computes the new command using system millis(). */
  float command(float measured) { return command(measured, millis()); }
  /** @brief Computes the new command using an explicit timestamp. */
  float command(float measured, unsigned now_msec);
  /** @brief Computes the command with an explicit derivative measurement. */
  float command(float measured, float d_measured, unsigned now_msec);

  FloatVariable& p() { return m_p; }                      ///< @return P gain variable.
  FloatVariable& i() { return m_i; }                      ///< @return I gain variable.
  FloatVariable& d() { return m_d; }                      ///< @return D gain variable.
  FloatVariable& i_max() { return m_i_max; }              ///< @return I-limit max variable.
  FloatVariable& i_min() { return m_i_min; }              ///< @return I-limit min variable.
  FloatVariable& feedforward() { return m_feedforward; }  ///< @return FF variable.
  FloatVariable& command_min() { return m_command_min; }  ///< @return Output clamp min variable.
  FloatVariable& command_max() { return m_command_max; }  ///< @return Output clamp max variable.

  FloatVariable& target() { return m_target; }      ///< @return Current setpoint variable.
  FloatVariable& d_target() { return m_d_target; }  ///< @return Target rate variable.

  float p_term() const { return m_p_cmd.value(); }
  float i_term() const { return m_i_cmd.value(); }
  float d_term() const { return m_d_cmd.value(); }
  float ff_term() const { return m_feedforward.value(); }

  VariableGroup& gains_vg() { return m_cfg_vg; }

 private:
  VariableGroup& m_cfg_vg;
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
