// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <Arduino.h>

#include <memory>
#include <vector>

#include "og3/compiler_definitions.h"
#include "og3/dependencies.h"
#include "og3/util.h"

namespace og3 {

class ModuleSystem;
class Logger;

// A Module is responsible for some part of an application's work.
// It works as part of an ModuleSystem (see module_system.h).
// It declares other modules it depends on, and which should run before it.
// It may register callbacks which the ModuleSystem will call during
//  the corresponding lifecycle of the application.
class Module {
 public:
  const char* name() const { return m_name; }

  Module(const Module&) = delete;

  ModuleSystem* module_system() { return m_module_system; }

 protected:
  Module(const char* name_, ModuleSystem* module_system);
  Logger* log() const;

  void add_link_fn(const LinkFn& link_fn);
  void add_init_fn(const Thunk& thunk);
  void add_start_fn(const Thunk& thunk);
  void add_update_fn(const Thunk& thunk);
  void add_html_button(String* body, const char* title, const char* url) const;
  unsigned sorted_index() const { return m_sorted_idx; }

 protected:
  template <class T>
  static T* GetModule(const NameToModule& name_to_module, const char* name) {
    auto iter = name_to_module.find(name);
    if (iter == name_to_module.end()) {
      return nullptr;
    }
    return reinterpret_cast<T*>(iter->second);
  }

  void set_sorted_idx(unsigned idx) { m_sorted_idx = idx; }

  void setDependencies(Dependencies* dependencies) { m_dependencies_ptr = dependencies; }
  void setDependencies(const std::vector<const char*>& dependencies);
  void setDependencies(const char* dependency);

 private:
  friend ModuleSystem;
  Dependencies* dependencies() { return m_dependencies_ptr; }
  const Dependencies* dependencies() const { return m_dependencies_ptr; }

  const char* m_name;
  Dependencies* m_dependencies_ptr = nullptr;
  std::unique_ptr<Dependencies> m_internal_dependencies;
  ModuleSystem* m_module_system;
  bool m_is_ok = false;
  unsigned m_sorted_idx = 0;
};

}  // namespace og3
