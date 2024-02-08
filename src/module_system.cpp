// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/module_system.h"

#include <algorithm>
#include <set>

#include "og3/logger.h"
#include "og3/module.h"

#define ALLOC(T, N) reinterpret_cast<T*>(alloca(N * sizeof(T)))

namespace og3 {

bool ModuleSystem::LinkFnRec::operator<(const ModuleSystem::LinkFnRec& o) const {
  return mod->sorted_index() < o.mod->sorted_index();
}

bool ModuleSystem::ThunkRec::operator<(const ModuleSystem::ThunkRec& o) const {
  return mod->sorted_index() < o.mod->sorted_index();
}

ModuleSystem::ModuleSystem(Logger* logger, unsigned reserve_num_modules) : m_logger(logger) {
  m_modules.reserve(reserve_num_modules);
}

void ModuleSystem::add_module(Module* module) { m_modules.push_back(module); }

void ModuleSystem::add_link_fn(const LinkFn& fn, Module* mod) { m_link_fns.emplace_back(fn, mod); }
void ModuleSystem::add_init_fn(const Thunk& thunk, Module* mod) {
  m_init_fns.emplace_back(thunk, mod);
}
void ModuleSystem::add_start_fn(const Thunk& thunk, Module* mod) {
  m_start_fns.emplace_back(thunk, mod);
}
void ModuleSystem::add_update_fn(const Thunk& thunk, Module* mod) {
  m_update_fns.emplace_back(thunk, mod);
}

bool ModuleSystem::setup() {
  if (!link()) {
    return false;
  }
  init();
  start();
  return true;
}

bool ModuleSystem::link() {
  // Allow modules to get pointers to one another by name, so they can declared
  //  their dependencies for the topological sort below.
  if (!link_modules_by_name()) {
    return false;
  }

  // Do a toplogical sort of the modules, such that dependencies for a given
  //  module will be before that module.
  // The output is a list of indexes into m_modules such that:
  //  module[sorted_module_indexes[i]] <  module[sorted_module_indexes[i+1]]
  size_t* sorted_module_indexes = ALLOC(size_t, m_modules.size());
  if (!topological_sort(sorted_module_indexes)) {
    return false;
  }

  // Tell each module what its index in the sorted list is.
  for (unsigned i = 0; i < m_modules.size(); i++) {
    m_modules[sorted_module_indexes[i]]->set_sorted_idx(i);
  }

  // Sort the init, start, and update functions by the sorted module index.
  // There is no need to sort the link functions because they are already sorted.
  // Not every module has an init, start, and update function, and there may be
  //  more than one of these funtions per module.
  // Using a stable sort means that the functions of a given type (init, start, update)
  //  for a given module remains the order in which the module added them.
  std::stable_sort(m_init_fns.begin(), m_init_fns.end());
  std::stable_sort(m_start_fns.begin(), m_start_fns.end());
  std::stable_sort(m_update_fns.begin(), m_update_fns.end());

  m_is_ok = true;
  return true;
}

// Allow modules to grab pointers to one another by name.
bool ModuleSystem::link_modules_by_name() {
  const size_t n_modules = m_modules.size();
  std::map<const char*, Module*> name_to_module;
  for (size_t i = 0; i < n_modules; i++) {
    name_to_module[m_modules[i]->name()] = m_modules[i];
  }
  for (const auto& link_fn : m_link_fns) {
    if (link_fn.fn && !link_fn.fn(name_to_module)) {
      return false;
    }
  }
  // Resolve dependencies after linking so modules can use pointers gathered during linking.
  for (auto mod : m_modules) {
    if (mod->dependencies()) {
      mod->dependencies()->resolve(name_to_module);
    }
  }
  return true;
}

// Order the execution of modules based on dependencies.
bool ModuleSystem::topological_sort(size_t* out_sorted_module_indexes) {
  const size_t n_modules = m_modules.size();
  std::map<const Module*, size_t> module_to_index;
  bool* visiting = ALLOC(bool, n_modules);
  std::set<size_t> unsorted;

  for (size_t i = 0; i < n_modules; i++) {
    visiting[i] = false;
    unsorted.insert(i);
    module_to_index[m_modules[i]] = i;
  }

  std::function<bool(size_t)> visit = [this, &module_to_index, n_modules, out_sorted_module_indexes,
                                       &unsorted, &visit, visiting](size_t idx) -> bool {
    if (unsorted.find(idx) == unsorted.end()) {
      return true;
    }
    const Module* module = m_modules[idx];
    if (visiting[idx]) {
      log()->logf("Dependency loop detected at module '%s'.", module->name());
      return false;
    }
    visiting[idx] = true;
    if (module->dependencies()) {
      const size_t num_preds = module->dependencies()->num_depends_on();
      for (size_t pred_idx = 0; pred_idx < num_preds; pred_idx++) {
        const Module* pred = module->dependencies()->depends_on(pred_idx);
        if (pred && !visit(module_to_index[pred])) {
          return false;
        }
      }
    }
    visiting[idx] = false;
    const size_t sidx = n_modules - unsorted.size();
    out_sorted_module_indexes[sidx] = idx;
    unsorted.erase(idx);
    return true;
  };

  while (!unsorted.empty()) {
    if (!visit(*unsorted.begin())) {
      return false;
    }
  }
  return true;
}

void ModuleSystem::init() {
  if (!m_is_ok) {
    return;
  }
  for (auto& fn : m_init_fns) {
    fn.fn();
  }
}
void ModuleSystem::start() {
  if (!m_is_ok) {
    return;
  }
  for (auto& fn : m_start_fns) {
    fn.fn();
  }
}
int ModuleSystem::update() {
  if (!m_is_ok) {
    return -1;
  }
  for (auto& fn : m_update_fns) {
    fn.fn();
  }
  return m_update_fns.size();
}

}  // namespace og3
