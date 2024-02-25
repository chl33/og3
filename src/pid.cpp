// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/pid.h"

namespace og3 {

namespace {
constexpr unsigned kCfgSet = VariableBase::Flags::kConfig | VariableBase::Flags::kSettable;
constexpr unsigned kOutputSet = 0;
}  // namespace

PID::PID(const PID::Gains& gains, VariableGroup* vg, VariableGroup* cfg_vg, VariableGroup* cmd_vg)
    : m_p("p", gains.p, "", "proportional gain", kCfgSet, 2, cfg_vg),
      m_i("i", gains.i, "", "integral gain", kCfgSet, 5, cfg_vg),
      m_d("d", gains.d, "", "derivative gain", kCfgSet, 2, cfg_vg),
      m_i_max("i_max", gains.i_max, "", "maximum integral val", kCfgSet, 1, cfg_vg),
      m_i_min("i_min", gains.i_min, "", "minimum integral val", kCfgSet, 1, cfg_vg),
      m_feedforward("feedforward", gains.feedforward, "", "feedforward", kCfgSet, 1, cfg_vg),
      m_command_min("command_min", gains.command_min, "", "minimum output", kCfgSet, 1, cfg_vg),
      m_command_max("command_max", gains.command_max, "", "maximum output", kCfgSet, 1, cfg_vg),
      m_last_msec(millis()),
      m_target("target", 25.0f, "", "control target", kCfgSet, 1, cmd_vg),
      m_d_target("d_target", 0.0f, "", "control target derivative", VariableBase::Flags::kConfig, 1,
                 cmd_vg),
      m_error("error", 0.0f, "", "error", kOutputSet, 1, vg),
      m_d_error("d_error", 0.0f, "", "derivative error", kOutputSet, 1, vg),
      m_command("command", 0.0f, "", "control output", kOutputSet, 1, vg),
      m_p_cmd("p_cmd", 0.0f, "", "proportional command component", kOutputSet, 1, vg),
      m_d_cmd("d_cmd", 0.0f, "", "derivative command component", kOutputSet, 1, vg),
      m_i_cmd("i_cmd", 0.0f, "", "integral command component", kOutputSet, 1, vg) {}

float PID::command(float measured, unsigned now_msec) { return command(measured, 0.0f, now_msec); }

float PID::command(float measured, float d_measured, unsigned now_msec) {
  auto command_is_saturated = [this]() {
    if (m_error.value() >= 0) {
      const float saturation = m_command_max.value() * 0.99 + m_command_min.value() * 0.01;
      return m_command.value() > saturation;
    } else {
      const float saturation = m_command_max.value() * 0.01 + m_command_min.value() * 0.99;
      return m_command.value() < saturation;
    }
  };

  const int d_msec = now_msec - m_last_msec;
  m_last_msec = now_msec;

  m_error = m_target.value() - measured;
  m_d_error = m_d_target.value() - d_measured;

  // Only update integral if command is not saturated in the same direction as the error.
  if (!command_is_saturated()) {
    m_i_cmd = m_i_cmd.value() + m_error.value() * d_msec * 1e-3 * m_i.value();
    if (m_i_cmd.value() < m_i_min.value()) {
      m_i_cmd = m_i_min.value();
    } else if (m_i_cmd.value() > m_i_max.value()) {
      m_i_cmd = m_i_max.value();
    }
  }
  m_p_cmd = m_error.value() * m_p.value();
  m_d_cmd = m_d_error.value() * m_d.value();
  m_command = m_p_cmd.value() + m_d_cmd.value() + m_i_cmd.value() + m_feedforward.value();
  if (m_command.value() < m_command_min.value()) {
    m_command = m_command_min.value();
  } else if (m_command.value() > m_command_max.value()) {
    m_command = m_command_max.value();
  }
  return m_command.value();
}

}  // namespace og3
