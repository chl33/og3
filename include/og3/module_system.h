// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <functional>
#include <vector>

#include "og3/compiler_definitions.h"
#include "og3/util.h"

namespace og3 {

class Logger;
class Module;

/**
 * @brief Manages a collection of Modules, orchestrating their lifecycle, dependencies, and
 * execution order.
 *
 * The ModuleSystem is central to the og3 application framework, enabling applications to be built
 * from a set of modular, interdependent components. It handles the registration, linking,
 * sorting, and execution of module-defined callbacks across the application's lifecycle.
 */
class ModuleSystem {
 public:
  /**
   * @brief Constructs a ModuleSystem instance.
   *
   * @param logger A pointer-to-pointer to the Logger. This allows the logger to be dynamically
   *               swapped (e.g., from Serial to UDP) by the App during its initialization.
   * @param reserve_num_modules Initial capacity for the internal module storage.
   */
  ModuleSystem(Logger** logger, unsigned reserve_num_modules = 16);

  /**
   * @brief Adds a Module to the ModuleSystem.
   *
   * Modules typically call this method from their constructors to register themselves.
   * @param module A pointer to the Module to add.
   */
  void add_module(Module* module);

  /**
   * @brief Prepares the ModuleSystem and all its registered Modules for execution.
   *
   * This function performs the linking, dependency resolution, sorting, initialization,
   * and starting phases of the module lifecycle. It should be called once after all
   * modules have been constructed, typically in the Arduino `setup()` function.
   * @return `true` if setup was successful, `false` otherwise (e.g., due to circular dependencies).
   */
  bool setup();

  /**
   * @brief Executes the update callbacks for all registered modules.
   *
   * This function should be called repeatedly in the main application loop (e.g., Arduino
   * `loop()`). Module update callbacks are invoked in a topologically sorted order.
   * @return The number of update callbacks executed, or -1 if the system is not initialized
   * (`!ok()`).
   */
  int update();

  /**
   * @brief Performs the linking phase of the module lifecycle.
   *
   * This allows modules to discover and store pointers to other modules and declare their
   * dependencies. Called internally by `setup()`.
   * @return `true` if linking was successful, `false` otherwise.
   */
  bool link();

  /**
   * @brief Checks if the ModuleSystem is in a good operational state.
   * @return `true` if the system is initialized and ready, `false` otherwise.
   */
  bool ok() const { return m_is_ok; }

  /**
   * @brief Executes the initialization callbacks for all registered modules.
   *
   * Callbacks are invoked in a topologically sorted order. Called internally by `setup()`.
   */
  void init();

  /**
   * @brief Executes the start callbacks for all registered modules.
   *
   * Callbacks are invoked in a topologically sorted order. Called internally by `setup()`.
   */
  void start();

  /**
   * @brief Gets the logger instance used by the ModuleSystem.
   * @return Pointer to the Logger.
   */
  Logger* log() { return *m_logger; }

  /**
   * @brief Returns the number of modules currently registered.
   * @return The count of registered modules.
   */
  size_t num_modules() const { return m_modules.size(); }

  /**
   * @brief Returns the allocated capacity for modules.
   * @return The module capacity.
   */
  size_t module_capacity() const { return m_modules.capacity(); }

 private:
  friend class Module;  ///< @brief Module is a friend to allow it to add callbacks.

  /**
   * @brief Adds a linking callback. Called by Modules during their construction.
   * @param link_fn The callback function.
   * @param mod The module registering the callback.
   */
  void add_link_fn(const LinkFn& link_fn, Module* mod);

  /**
   * @brief Adds an initialization callback. Called by Modules during their construction.
   * @param thunk The callback function.
   * @param mod The module registering the callback.
   */
  void add_init_fn(const Thunk& thunk, Module* mod);

  /**
   * @brief Adds a start callback. Called by Modules during their construction.
   * @param thunk The callback function.
   * @param mod The module registering the callback.
   */
  void add_start_fn(const Thunk& thunk, Module* mod);

  /**
   * @brief Adds an update callback. Called by Modules during their construction.
   * @param thunk The callback function.
   * @param mod The module registering the callback.
   */
  void add_update_fn(const Thunk& thunk, Module* mod);

  /**
   * @brief Allows modules to resolve pointers to other modules by name.
   * @return `true` if all link functions execute successfully.
   */
  bool link_modules_by_name();

  /**
   * @brief Performs a topological sort of modules based on their declared dependencies.
   * @param sorted_module_indexes An output array to store the sorted indices of modules.
   * @return `true` if sorting was successful, `false` if a circular dependency was detected.
   */
  bool topological_sort(size_t* sorted_module_indexes);

  Logger** m_logger;     ///< @brief Pointer to the application's Logger pointer.
  bool m_is_ok = false;  ///< @brief Flag indicating the operational status of the ModuleSystem.
  std::vector<Module*> m_modules;  ///< @brief Collection of all registered modules.

  /// @brief Helper struct to pair a LinkFn with its owning Module.
  struct LinkFnRec {
    LinkFnRec(const LinkFn& f, Module* m) : fn(f), mod(m) {}
    LinkFnRec(const LinkFnRec&) = default;
    LinkFn fn;    ///< The link callback function.
    Module* mod;  ///< The module that owns this callback.
    bool operator<(const LinkFnRec& o) const;
  };
  /// @brief Helper struct to pair a Thunk (init, start, update callback) with its owning Module.
  struct ThunkRec {
    ThunkRec(const Thunk& f, Module* m) : fn(f), mod(m) {}
    ThunkRec(const ThunkRec&) = default;
    Thunk fn;     ///< The callback function.
    Module* mod;  ///< The module that owns this callback.
    bool operator<(const ThunkRec& o) const;
  };

  std::vector<LinkFnRec> m_link_fns;   ///< @brief List of all registered link functions.
  std::vector<ThunkRec> m_init_fns;    ///< @brief List of all registered init functions.
  std::vector<ThunkRec> m_start_fns;   ///< @brief List of all registered start functions.
  std::vector<ThunkRec> m_update_fns;  ///< @brief List of all registered update functions.};

}  // namespace og3
