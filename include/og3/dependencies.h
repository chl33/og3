// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <array>
#include <vector>

#include "og3/util.h"

namespace og3 {

// This interface communicates which modules a given module depends upon.
class Dependencies {
 public:
  virtual bool resolve(const NameToModule& name_to_module) = 0;
  virtual size_t num_depends_on() const = 0;
  virtual const Module* depends_on(size_t idx) const = 0;
};

// DependenciesArray specifies a set of dependencies via a static list of names.
template <unsigned N>
class DependenciesArray : public Dependencies {
 public:
  explicit DependenciesArray(const std::array<const char*, N>& module_names,
                             Dependencies* prev_dependencies = nullptr)
      : m_prev(prev_dependencies),
        m_prev_num(prev_dependencies ? prev_dependencies->num_depends_on() : 0),
        m_module_names(module_names) {}
  size_t num_depends_on() const final { return m_prev_num + m_modules.size(); }
  const Module* depends_on(size_t idx) const final {
    if (idx < m_prev_num) {
      return m_prev->depends_on(idx);
    }
    return m_modules[idx - m_prev_num];
  }
  bool resolve(const NameToModule& name_to_module) final {
    m_ok = m_prev ? m_prev->resolve(name_to_module) : true;
    for (size_t i = 0; i < m_module_names.size(); i++) {
      const auto name = m_module_names[i];
      if (!name) {
        m_modules[i] = nullptr;
        continue;
      }
      const auto it = name_to_module.find(name);
      if (it == name_to_module.end()) {
        m_ok = false;
        m_modules[i] = nullptr;
      } else {
        m_modules[i] = it->second;
      }
    }
    return m_ok;
  }
  bool is_ok() const { return m_ok; }

 private:
  Dependencies* m_prev;
  const size_t m_prev_num;
  const std::array<const char*, N> m_module_names;
  std::array<Module*, N> m_modules;
  bool m_ok = false;
};

// SingleDependcy specifies a single dependency with a static value.
class SingleDependency : public Dependencies {
 public:
  explicit SingleDependency(const char* name, Dependencies* prev_dependencies = nullptr)
      : m_name(name),
        m_prev(prev_dependencies),
        m_prev_num(prev_dependencies ? prev_dependencies->num_depends_on() : 0) {}
  size_t num_depends_on() const final { return m_prev_num + 1; }
  const Module* depends_on(size_t idx) const final {
    if (idx < m_prev_num) {
      return m_prev->depends_on(idx);
    }
    return m_module;
  }
  bool resolve(const NameToModule& name_to_module) final {
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
  bool is_ok() const { return m_ok; }

 private:
  const char* m_name;
  Dependencies* m_prev;
  const size_t m_prev_num;
  Module* m_module = nullptr;
  bool m_ok = false;
};

// DependenciesArray specifies a set of dependencies via a vector of module names.
class DependenciesVector : public Dependencies {
 public:
  explicit DependenciesVector(const std::vector<const char*>& module_names,
                              Dependencies* prev_dependencies = nullptr)
      : m_prev(prev_dependencies),
        m_prev_num(prev_dependencies ? prev_dependencies->num_depends_on() : 0),
        m_module_names(module_names),
        m_modules(module_names.size()) {}
  size_t num_depends_on() const final { return m_prev_num + m_modules.size(); }
  const Module* depends_on(size_t idx) const final {
    if (idx < m_prev_num) {
      return m_prev->depends_on(idx);
    }
    return m_modules[idx - m_prev_num];
  }
  bool resolve(const NameToModule& name_to_module) final {
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
  bool is_ok() const { return m_ok; }

 private:
  Dependencies* m_prev;
  const size_t m_prev_num;
  std::vector<const char*> m_module_names;
  std::vector<Module*> m_modules;
  bool m_ok = false;
};

}  // namespace og3
