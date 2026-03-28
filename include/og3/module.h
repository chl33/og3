// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once
#include <Arduino.h>

#include <memory>
#include <vector>

#include "og3/compiler_definitions.h"
#include "og3/module_system.h"
#include "og3/util.h"

namespace og3 {

class Logger;

/**
 * @brief Base class for all modules in the og3 application framework.
 *
 * A module is a discrete functional component of the application. The Module system
 * provides a structured lifecycle management (linking, initialization, start, update)
 * and handles dependency ordering between modules.
 */
class Module {
 public:
  Module(const Module&) = delete;

  /**
   * @brief Returns the name of the module.
   * @return The unique module name.
   */
  const char* name() const { return m_name; }

  /**
   * @brief Constructs a new Module instance.
   * @param name_ Unique name identifying the module.
   * @param module_system The ModuleSystem to register with.
   */
  Module(const char* name_, ModuleSystem* module_system);

  /**
   * @brief Gets the logger instance from the ModuleSystem.
   * @return Pointer to the Logger.
   */
  Logger* log() const;

  /** @return Pointer to the ModuleSystem this module belongs to. */
  ModuleSystem* module_system() const { return m_module_system; }

  /**
   * @brief Registers an initialization callback function.
   *
   * Initialization callbacks are executed after linking, in a topologically sorted order
   * based on declared dependencies. This is typically used for hardware setup (pin modes),
   * memory allocation, and data structure initialization.
   * @param thunk The callback function.
   */
  void add_init_fn(const Thunk& thunk);

  /**
   * @brief Registers a start callback function.
   *
   * Start callbacks are executed after all modules have been initialized, again in sorted order.
   * This is used for starting tasks, initial outputs, network operations, etc.
   * @param thunk The callback function.
   */
  void add_start_fn(const Thunk& thunk);

  /**
   * @brief Registers a function to be called in every iteration of the main loop.
   * @param thunk The callback function.
   */
  void add_update_fn(const Thunk& thunk);

  /**
   * @brief Adds an HTML button snippet to a String body.
   * @param body Pointer to the String where the HTML will be appended.
   * @param title The text displayed on the button.
   * @param url The URL the button points to.
   */
  void add_html_button(String* body, const char* title, const char* url) const;

  /**
   * @brief Retrieves the sorted index of this module within the ModuleSystem.
   * @return The sorted index.
   */
  unsigned sorted_index() const { return m_sorted_idx; }

 protected:
  /**
   * @brief Declaratively require another module.
   *
   * Registers a dependency and automatically populates the pointer during the linking phase.
   * @tparam T The type of the module.
   * @param name The name of the required module.
   * @param ptr Address of the pointer to populate.
   */
  template <class T>
  void require(const char* name, T** ptr) {
    m_module_system->add_requirement(this, name, reinterpret_cast<void**>(ptr));
  }

  /**
   * @brief Sets the sorted index of this module. Used internally by ModuleSystem.
   * @param idx The sorted index.
   */
  void set_sorted_idx(unsigned idx) { m_sorted_idx = idx; }

 private:
  friend ModuleSystem;  ///< @brief ModuleSystem is a friend to access private dependency
                        ///< information.

  const char* m_name;             ///< @brief Unique name identifying this module instance.
  ModuleSystem* m_module_system;  ///< @brief The ModuleSystem this module is part of.
  bool m_is_ok = false;           ///< @brief Indicates if the module is in a healthy state.
  unsigned m_sorted_idx = 0;      ///< @brief The topological sort index assigned by ModuleSystem.
};

}  // namespace og3
