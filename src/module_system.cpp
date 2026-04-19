// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/module_system.h"

#include <algorithm>
#include <set>

#include "og3/logger.h"
#include "og3/module.h"

namespace og3 {

ModuleSystem::ModuleSystem(Logger** logger, unsigned reserve_num_modules) : m_logger(logger) {
  m_modules.reserve(reserve_num_modules);
}

void ModuleSystem::add_module(Module* module) { m_modules.push_back(module); }

void ModuleSystem::add_init_fn(const Thunk& fn, Module* mod) { m_init_fns.emplace_back(fn, mod); }
void ModuleSystem::add_start_fn(const Thunk& fn, Module* mod) { m_start_fns.emplace_back(fn, mod); }
void ModuleSystem::add_update_fn(const Thunk& fn, Module* mod) {
  m_update_fns.emplace_back(fn, mod);
}

bool ModuleSystem::setup() {
  if (!link()) {
    return false;
  }
  init();
  start();

  m_pending_requirements.clear();
  m_pending_requirements.shrink_to_fit();
  m_implicit_deps.clear();
  m_implicit_deps.shrink_to_fit();

  return true;
}

bool ModuleSystem::link() {
  // Map module names to module pointers.
  NameToModule name_to_module;
  for (auto* mod : m_modules) {
    if (!mod) {
      log()->log("ModuleSystem: Found nullptr in m_modules during link().");
      return false;
    }
    name_to_module[mod->name()] = mod;
  }

  // Resolve declarative requirements
  for (auto& req : m_pending_requirements) {
    auto it = name_to_module.find(req.required_name);
    if (it != name_to_module.end()) {
      if (!it->second) {
        log()->logf("ModuleSystem: it->second is nullptr for '%s'.", req.required_name);
        return false;
      }
      *(req.target_ptr) = it->second;
      m_implicit_deps.push_back({req.owner, it->second});
    } else {
      log()->logf("Failed to resolve requirement '%s' for module '%s'.", req.required_name,
                  req.owner->name());
      return false;
    }
  }

  // Order the modules based on their dependencies.
  std::vector<size_t> sorted_module_indexes;
  if (!topological_sort_internal(&sorted_module_indexes)) {
    return false;
  }

  // Assign sorted indexes to modules.
  for (size_t i = 0; i < sorted_module_indexes.size(); i++) {
    m_modules[sorted_module_indexes[i]]->set_sorted_idx(i);
  }

  // Re-sort the callback lists based on the sorted module order.
  auto sort_thunks = [this](std::vector<ThunkRec>& thunks) {
    std::sort(thunks.begin(), thunks.end(), [](const ThunkRec& a, const ThunkRec& b) {
      return a.mod->sorted_index() < b.mod->sorted_index();
    });
  };

  sort_thunks(m_init_fns);
  sort_thunks(m_start_fns);
  sort_thunks(m_update_fns);

  m_is_ok = true;
  return true;
}

void ModuleSystem::init() {
  if (!m_is_ok) {
    return;
  }
  for (const auto& fn : m_init_fns) {
    fn.fn();
  }
}

void ModuleSystem::start() {
  if (!m_is_ok) {
    return;
  }
  for (const auto& fn : m_start_fns) {
    fn.fn();
  }
}

int ModuleSystem::update() {
  if (!m_is_ok) {
    return -1;
  }
  for (const auto& fn : m_update_fns) {
    fn.fn();
  }
  return m_update_fns.size();
}

bool ModuleSystem::topological_sort(size_t* sorted_module_indexes) {
  std::vector<size_t> vec;
  if (!topological_sort_internal(&vec)) {
    return false;
  }
  for (size_t i = 0; i < vec.size(); i++) {
    sorted_module_indexes[i] = vec[i];
  }
  return true;
}

bool ModuleSystem::topological_sort_internal(std::vector<size_t>* out_sorted_module_indexes) {
  const size_t n_modules = m_modules.size();
  out_sorted_module_indexes->resize(n_modules);

  std::map<const Module*, size_t> module_to_index;
  for (size_t i = 0; i < n_modules; i++) {
    module_to_index[m_modules[i]] = i;
  }

  std::set<size_t> unsorted;
  for (size_t i = 0; i < n_modules; i++) {
    unsorted.insert(i);
  }

  std::vector<bool> visiting(n_modules, false);

  std::function<bool(size_t)> visit = [&](size_t idx) -> bool {
    if (visiting[idx]) {
      log()->logf("Circular dependency detected involving module '%s'.", m_modules[idx]->name());
      return false;
    }
    if (unsorted.find(idx) == unsorted.end()) {
      return true;
    }

    visiting[idx] = true;
    const Module* module = m_modules[idx];

    for (const auto& edge : m_implicit_deps) {
      if (edge.first == module) {
        if (!visit(module_to_index[edge.second])) {
          return false;
        }
      }
    }

    visiting[idx] = false;
    const size_t sidx = n_modules - unsorted.size();
    (*out_sorted_module_indexes)[sidx] = idx;
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

}  // namespace og3
