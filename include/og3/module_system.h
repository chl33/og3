// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <functional>
#include <vector>

#include "og3/compiler_definitions.h"
#include "og3/util.h"

namespace og3 {

class Logger;
class Module;

// ModuleSystem allows an application to be created from a set of modules.
//
// Modules map depend upon other modules, and declare these dependencies by name.
//
// Application setup:
//
// 1. Add modules to the ModuleSystem.
//    Upon their construction, modules add themselves to the ModuleSystem.
//
// 2. Start.
//    When all modules are added, call setup() on the depend system to prepare
//     the modules to run:
//    This would be done in the Arduino setup() function for example.
//    a. First, a map of {name -> Module} is passed to all modules so they can
//       get pointers to other modules in the application.
//    b. Next, the modules are sorted so that each module runs after all the
//       modules they depend on.
//    c. Any module which has registered an 'init()' function now has these
//        called in sorted-module order.
//    c. Any module which has registered a 'start()' function now has these
//        called in sorted-module order.
//
// 3. Run.
//    In the main loop of the application, call the ModuleSystem's update()
//     function.
//    On update(), each module which has registered an update() function will
//     have this called, in the sorted-module order.
//
class ModuleSystem {
 public:
  ModuleSystem(Logger* logger, unsigned reserve_num_modules = 16) RT3_NONNULL();

  void add_module(Module* module);

  bool setup();
  int update();

  // These are called by setup().
  bool link();
  bool ok() const { return m_is_ok; }
  void init();
  void start();

  Logger* log() { return m_logger; }

 private:
  friend class Module;
  // These are called by Modules during add_module().
  void add_link_fn(const LinkFn& link_fn, Module* mod);
  void add_init_fn(const Thunk& thunk, Module* mod);
  void add_start_fn(const Thunk& thunk, Module* mod);
  void add_update_fn(const Thunk& thunk, Module* mod);

  // Internal-only, used by link().
  bool link_modules_by_name();
  bool topological_sort(size_t* sorted_module_indexes);

  Logger* m_logger;
  bool m_is_ok = false;
  std::vector<Module*> m_modules;

  struct LinkFnRec {
    LinkFnRec(const LinkFn& f, Module* m) : fn(f), mod(m) {}
    LinkFnRec(const LinkFnRec&) = default;
    LinkFn fn;
    Module* mod;
    bool operator<(const LinkFnRec& o) const;
  };
  struct ThunkRec {
    ThunkRec(const Thunk& f, Module* m) : fn(f), mod(m) {}
    ThunkRec(const ThunkRec&) = default;
    Thunk fn;
    Module* mod;
    bool operator<(const ThunkRec& o) const;
  };

  std::vector<LinkFnRec> m_link_fns;
  std::vector<ThunkRec> m_init_fns;
  std::vector<ThunkRec> m_start_fns;
  std::vector<ThunkRec> m_update_fns;
};

}  // namespace og3
