// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <array>
#include <cmath>
#include <cstring>
#include <vector>

#include "og3/module_system.h"
#include "og3/variable.h"

namespace og3 {

// KernelFilter supports getting inputs sampled over time and producing a synthetic
// reading of these inputs smoothed using a kernel filter.

// Base class for KernelFilter which does not depend on the size template parameter.
class KernelFilter {
 public:
  static constexpr unsigned kDefaultNumSamples = 30;

  struct Options {
    const char* name;         // The name of the output value variable
    const char* units;        // The units of the output
    const char* description;  // A description of the output variable
    unsigned var_flags;       // Variable flags for the output variable
    float sigma;              // The sigma of the gausian used for smoothing
    int decimals;             // The number of decimal values for printing the output value
    size_t size;              // The number of samples used for the smoothing kernel.
  };

  KernelFilter(const Options& options, ModuleSystem* module_system_, VariableGroup& vg);

  float value() const { return m_value.value(); }
  float kernelValue(float dt) const { return exp(m_exp_scalar * dt * dt); }
  size_t size() const { return m_values.size(); }
  void setSigma(float sigma);

  float addSample(float time, float value);
  float computeValue(float time) const;

  const FloatVariable& valueVariable() const { return m_value; }

  unsigned long numSamples() const { return m_num_samples; }

  // The state of the filter can be saved and restored.
  // This could be useful for restoring state after deep sleep etc...
  template <int SZ>
  struct State {
    unsigned long num_samples = 0;
    float sigma = 0;
    std::array<float, SZ> values{0.0f};
    std::array<float, SZ> times{0.0f};
  };

  template <int SZ>
  bool saveState(State<SZ>& state) {
    if (SZ != m_values.size()) {
      return false;
    }
    memcpy(state.values.data(), m_values.data(), sizeof(state.values[0]) * state.values.size());
    memcpy(state.times.data(), m_times.data(), sizeof(state.times[0]) * state.times.size());
    state.num_samples = m_num_samples;
    state.sigma = m_sigma;
    return true;
  }

  template <int SZ>
  bool restoreState(const State<SZ>& state) {
    if (SZ != m_values.size()) {
      return false;
    }
    memcpy(m_values.data(), state.values.data(), sizeof(state.values[0]) * state.values.size());
    memcpy(m_times.data(), state.times.data(), sizeof(state.times[0]) * state.times.size());
    m_num_samples = state.num_samples;
    m_sigma = state.sigma;
    return true;
  }

#if 0
  // Call in setup to suggest an icon for HomeAssistant discovery MQTT message.
  void setHAIcon(const char* icon) { m_icon = icon; }
#endif

 protected:
  FloatVariable m_value;
  unsigned long m_num_samples = 0;
  float m_sigma;
  float m_exp_scalar;
  std::vector<float> m_values;
  std::vector<float> m_times;
};

}  // namespace og3
