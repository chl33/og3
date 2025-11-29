// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/dependencies.h"

#include <array>
#include <cstddef>
#include <vector>

#include "og3/util.h"

namespace og3 {

const Module* SingleDependency::depends_on(size_t idx) const {
  if (idx < m_prev_num) {
    return m_prev->depends_on(idx);
  }
  return m_module;
}
bool SingleDependency::resolve(const NameToModule& name_to_module) {
  if (m_prev) {
    m_ok = m_prev->resolve(name_to_module);
  }
  if (!m_name) {
    m_ok = false;
    return true;
  }
  const auto iter = name_to_module.find(m_name);
  if (iter == name_to_module.end()) {
    m_ok = false;
    m_module = nullptr;
  } else {
    m_module = iter->second;
  }
  return m_ok;
}

const Module* DependenciesVector::depends_on(size_t idx) const {
  if (idx < m_prev_num) {
    return m_prev->depends_on(idx);
  }
  return m_modules[idx - m_prev_num];
}

bool DependenciesVector::resolve(const NameToModule& name_to_module) {
  m_ok = true;
  for (size_t i = 0; i < m_module_names.size(); i++) {
    const auto name = m_module_names[i];
    if (!name) {
      m_modules[i] = nullptr;
      continue;
    }
    auto iter = name_to_module.find(name);
    if (iter == name_to_module.end()) {
      m_ok = false;
      m_modules[i] = nullptr;
    } else {
      m_modules[i] = iter->second;
    }
  }
  return m_ok;
}

}  // namespace og3
