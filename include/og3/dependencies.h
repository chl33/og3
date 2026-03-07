// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include "og3/util.h"

namespace og3 {

class Module;

/**
 * @brief Interface for communicating which modules a given module depends upon.
 *
 * This class is used by the ModuleSystem to resolve dependencies and determine
 * the correct initialization, start, and update order for all modules.
 */
class Dependencies {
 public:
  virtual ~Dependencies() = default;

  /**
   * @brief Resolves module names to actual module pointers.
   * @param name_to_module A map of module names to their pointers.
   * @return true if all dependencies were successfully resolved.
   */
  virtual bool resolve(const NameToModule& name_to_module) = 0;

  /**
   * @brief Returns the total number of modules this object depends on.
   * @return The dependency count.
   */
  virtual size_t num_depends_on() const = 0;

  /**
   * @brief Returns a pointer to a dependent module by index.
   * @param idx The index of the dependency (0 to num_depends_on() - 1).
   * @return Constant pointer to the dependent Module.
   */
  virtual const Module* depends_on(size_t idx) const = 0;
};

/**
 * @brief Specifies a set of dependencies via a static array of module names.
 * @tparam N The number of dependencies in the array.
 */
template <unsigned N>
class DependenciesArray : public Dependencies {
 public:
  /**
   * @brief Constructs a DependenciesArray.
   * @param module_names A static array of unique module names.
   * @param prev_dependencies Optional pointer to another Dependencies object to chain.
   */
  explicit DependenciesArray(const std::array<const char*, N>& module_names,
                             Dependencies* prev_dependencies = nullptr)
      : m_prev(prev_dependencies),
        m_prev_num(prev_dependencies ? prev_dependencies->num_depends_on() : 0),
        m_module_names(module_names) {}

  /** @return Total number of dependencies, including any chained ones. */
  size_t num_depends_on() const final { return m_prev_num + m_modules.size(); }

  /**
   * @brief Returns a dependent module by index.
   * @param idx The dependency index.
   * @return Pointer to the Module.
   */
  const Module* depends_on(size_t idx) const final {
    if (idx < m_prev_num) {
      return m_prev->depends_on(idx);
    }
    return m_modules[idx - m_prev_num];
  }

  /**
   * @brief Resolves all names in the array to module pointers.
   * @param name_to_module Map of names to module pointers.
   * @return true if all resolutions (including chained ones) succeeded.
   */
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

  /** @return true if all dependencies were successfully resolved. */
  bool is_ok() const { return m_ok; }

 private:
  Dependencies* m_prev;
  const size_t m_prev_num;
  const std::array<const char*, N> m_module_names;
  std::array<Module*, N> m_modules;
  bool m_ok = false;
};

/**
 * @brief Specifies a single dependency by name.
 */
class SingleDependency : public Dependencies {
 public:
  /**
   * @brief Constructs a SingleDependency.
   * @param name The name of the required module.
   * @param prev_dependencies Optional pointer to another Dependencies object to chain.
   */
  explicit SingleDependency(const char* name, Dependencies* prev_dependencies = nullptr)
      : m_name(name),
        m_prev(prev_dependencies),
        m_prev_num(prev_dependencies ? prev_dependencies->num_depends_on() : 0) {}

  size_t num_depends_on() const final { return m_prev_num + 1; }
  const Module* depends_on(size_t idx) const final;
  bool resolve(const NameToModule& name_to_module) final;
  /** @return true if the dependency was successfully resolved. */
  bool is_ok() const { return m_ok; }

 private:
  const char* m_name;
  Dependencies* m_prev;
  const size_t m_prev_num;
  Module* m_module = nullptr;
  bool m_ok = false;
};

/**
 * @brief Specifies a set of dependencies via a dynamic vector of module names.
 */
class DependenciesVector : public Dependencies {
 public:
  /**
   * @brief Constructs a DependenciesVector.
   * @param module_names A vector of unique module names.
   * @param prev_dependencies Optional pointer to another Dependencies object to chain.
   */
  explicit DependenciesVector(const std::vector<const char*>& module_names,
                              Dependencies* prev_dependencies = nullptr)
      : m_prev(prev_dependencies),
        m_prev_num(prev_dependencies ? prev_dependencies->num_depends_on() : 0),
        m_module_names(module_names),
        m_modules(module_names.size()) {}

  size_t num_depends_on() const final { return m_prev_num + m_modules.size(); }
  const Module* depends_on(size_t idx) const final;
  bool resolve(const NameToModule& name_to_module) final;
  /** @return true if all dependencies were successfully resolved. */
  bool is_ok() const { return m_ok; }

 private:
  Dependencies* m_prev;
  const size_t m_prev_num;
  std::vector<const char*> m_module_names;
  std::vector<Module*> m_modules;
  bool m_ok = false;
};

}  // namespace og3
