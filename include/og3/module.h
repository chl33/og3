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

/**
 * @brief Base class for all modules in the og3 application framework.
 *
 * A Module represents a distinct functional unit within an application. It integrates
 * with a ModuleSystem, declares its dependencies on other modules, and registers
 * callbacks for various stages of the application's lifecycle (initialization, start, update).
 */
class Module {
 public:
  /**
   * @brief Returns the unique name of this module.
   * @return The module's name.
   */
  const char* name() const { return m_name; }

  // Prevent copying of Modules
  Module(const Module&) = delete;

  /**
   * @brief Gets a pointer to the ModuleSystem this module belongs to.
   * @return Pointer to the ModuleSystem.
   */
  ModuleSystem* module_system() { return m_module_system; }

 protected:
  /**
   * @brief Constructs a new Module instance.
   *
   * Automatically registers this module with the provided ModuleSystem.
   * @param name_ A unique name for this module.
   * @param module_system The ModuleSystem this module is part of.
   */
  Module(const char* name_, ModuleSystem* module_system);

  /**
   * @brief Gets the logger instance from the ModuleSystem.
   * @return Pointer to the Logger.
   */
  Logger* log() const;

  /**
   * @brief Registers a linking callback function.
   *
   * Linking callbacks are executed early in the application lifecycle, allowing modules
   * to discover and store pointers to other modules they depend on.
   * @param link_fn The callback function.
   */
  void add_link_fn(const LinkFn& link_fn);

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
   * @brief Registers an update callback function.
   *
   * Update callbacks are executed repeatedly in the main application loop, in sorted order.
   * This is where the continuous work of the application is performed.
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
   * @brief Helper function to retrieve a module by name and cast it to the correct type.
   * @tparam T The expected type of the module.
   * @param name_to_module The map of module names to module pointers.
   * @param name The name of the module to retrieve.
   * @return A pointer to the module, or nullptr if not found or wrong type.
   */
  template <class T>
  static T* GetModule(const NameToModule& name_to_module, const char* name) {
    auto iter = name_to_module.find(name);
    if (iter == name_to_module.end()) {
      return nullptr;
    }
    return reinterpret_cast<T*>(iter->second);
  }

  /**
   * @brief Sets the sorted index of this module. Used internally by ModuleSystem.
   * @param idx The sorted index.
   */
  void set_sorted_idx(unsigned idx) { m_sorted_idx = idx; }

  /**
   * @brief Sets the Dependencies object for this module.
   * @param dependencies Pointer to the Dependencies object (owned externally).
   */
  void setDependencies(Dependencies* dependencies) { m_dependencies_ptr = dependencies; }

  /**
   * @brief Declares a list of dependencies for this module.
   *
   * The ModuleSystem will ensure that modules in the list are initialized before this module.
   * @param dependencies A vector of C-style strings, where each string is the name of a dependent
   * module.
   */
  void setDependencies(const std::vector<const char*>& dependencies);

  /**
   * @brief Declares a single dependency for this module.
   *
   * The ModuleSystem will ensure that the dependent module is initialized before this module.
   * @param dependency The name of the dependent module.
   */
  void setDependencies(const char* dependency);

 private:
  friend ModuleSystem;  ///< @brief ModuleSystem is a friend to access private dependency
                        ///< information.
  /**
   * @brief Gets a pointer to the Dependencies object for this module.
   * @return Pointer to Dependencies.
   */
  Dependencies* dependencies() { return m_dependencies_ptr; }
  /**
   * @brief Gets a constant pointer to the Dependencies object for this module.
   * @return Constant pointer to Dependencies.
   */
  const Dependencies* dependencies() const { return m_dependencies_ptr; }

  const char* m_name;  ///< @brief The unique name of this module.
  Dependencies* m_dependencies_ptr =
      nullptr;  ///< @brief Raw pointer to the Dependencies object (can be internal or external).
  std::unique_ptr<Dependencies>
      m_internal_dependencies;    ///< @brief Owned Dependencies object if managed internally.
  ModuleSystem* m_module_system;  ///< @brief The ModuleSystem this module is part of.
  bool m_is_ok = false;           ///< @brief Indicates if the module is in a healthy state.
  unsigned m_sorted_idx = 0;      ///< @brief The topological sort index assigned by ModuleSystem.
