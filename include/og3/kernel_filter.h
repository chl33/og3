// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <array>
#include <cinttypes>
#include <cmath>
#include <cstring>
#include <vector>

#include "og3/module_system.h"
#include "og3/variable.h"

namespace og3 {

/**
 * @brief Implements a kernel-based smoothing filter for time-series data.
 *
 * KernelFilter collects samples over time and computes a weighted average
 * using a Gaussian kernel. This is effective for smoothing noisy sensor readings.
 */
class KernelFilter {
 public:
  static constexpr unsigned kDefaultNumSamples = 30;  ///< @brief Default buffer size.

  /** @brief Configuration options for the filter. */
  struct Options {
    const char* name;         ///< The name of the output value variable.
    const char* units;        ///< The units of the output.
    const char* description;  ///< A description of the output variable.
    unsigned var_flags;       ///< Variable flags for the output variable.
    float sigma;              ///< The sigma of the Gaussian used for smoothing.
    int decimals;             ///< Decimal places for display.
    size_t size;              ///< Buffer size (number of samples).
  };

  /** @brief Constructs a KernelFilter. */
  KernelFilter(const Options& options, ModuleSystem* module_system_, VariableGroup& vg);

  /** @return The current smoothed value. */
  float value() const { return m_value.value(); }
  /** @return The weight contribution for a given time delta. */
  float kernelValue(float dt) const { return exp(m_exp_scalar * dt * dt); }
  /** @return The current buffer capacity. */
  size_t size() const { return m_values.size(); }
  /** @brief Updates the smoothing sigma. */
  void setSigma(float sigma);

  /**
   * @brief Adds a new sample to the filter.
   * @param time The timestamp of the sample (in seconds).
   * @param value The raw sample value.
   * @return The updated filtered value.
   */
  float addSample(float time, float value);

  /**
   * @brief Computes the smoothed value for a specific time.
   * @param time The evaluation timestamp.
   * @return The filtered value.
   */
  float computeValue(float time) const;

  /** @return Constant reference to the filtered value variable. */
  const FloatVariable& valueVariable() const { return m_value; }

  /** @return Total number of samples added since initialization. */
  unsigned long numSamples() const { return m_num_samples; }

  /** @brief Serializable state of the filter. */
  template <int SZ>
  struct State {
    static constexpr uint32_t kHeader = 0x4321;
    uint32_t header;
    unsigned long num_samples;
    float sigma;
    std::array<float, SZ> values;
    std::array<float, SZ> times;
  };

  /** @brief Saves filter state for persistence (e.g. deep sleep). */
  template <int SZ>
  bool saveState(State<SZ>& state) {
    if (SZ != m_values.size()) {
      return false;
    }
    state.header = State<SZ>::kHeader;
    memcpy(state.values.data(), m_values.data(), sizeof(state.values[0]) * state.values.size());
    memcpy(state.times.data(), m_times.data(), sizeof(state.times[0]) * state.times.size());
    state.num_samples = m_num_samples;
    state.sigma = m_sigma;
    return true;
  }

  /** @brief Restores filter state from a buffer. */
  template <int SZ>
  bool restoreState(const State<SZ>& state) {
    if (SZ != m_values.size()) {
      return false;
    }
    if (state.header != State<SZ>::kHeader) {
      return false;
    }
    memcpy(m_values.data(), state.values.data(), sizeof(state.values[0]) * state.values.size());
    memcpy(m_times.data(), state.times.data(), sizeof(state.times[0]) * state.times.size());
    m_num_samples = state.num_samples;
    m_sigma = state.sigma;
    return true;
  }

 protected:
  FloatVariable m_value;
  unsigned long m_num_samples = 0;
  float m_sigma;
  float m_exp_scalar;
  std::vector<float> m_values;
  std::vector<float> m_times;
};

}  // namespace og3
