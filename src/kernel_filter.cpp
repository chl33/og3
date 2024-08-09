// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/kernel_filter.h"

namespace og3 {

KernelFilter::KernelFilter(const Options& options, ModuleSystem* module_system_, VariableGroup& vg)
    : m_value(options.name, 0.0f, options.units, options.description, options.var_flags,
              options.decimals, vg),
      m_sigma(-100.0f),
      m_exp_scalar(0.0f),
      m_values(options.size),
      m_times(options.size) {
  setSigma(options.sigma);
}

void KernelFilter::setSigma(float sigma) {
  if (sigma == m_sigma) {
    return;
  }
  m_sigma = sigma;
  m_exp_scalar = -0.5 / (sigma * sigma);
}

float KernelFilter::addSample(float time, float value) {
  const unsigned idx = m_num_samples % size();
  m_times[idx] = time;
  m_values[idx] = value;
  m_num_samples += 1;
  m_value = computeValue(time);
  return m_value.value();
}

float KernelFilter::computeValue(float time) const {
  const unsigned first_index = m_num_samples <= size() ? 0 : m_num_samples - size();
  const unsigned last_index = m_num_samples;
  float total_weight = 0.0f;
  float total_weighted_values = 0.0f;

  for (unsigned i = first_index; i < last_index; i++) {
    const unsigned idx = i % size();
    const float dt = time - m_times[idx];
    const float weight = kernelValue(dt);
    total_weight += weight;
    total_weighted_values += weight * m_values[idx];
  }
  return total_weighted_values / total_weight;
}

}  // namespace og3
