// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/logger.h"
#include "og3/module_system.h"
#include "og3/serial_logger.h"
#include "og3/tasks.h"

namespace og3 {

/**
 * @brief The main application class, acting as a container for modules, tasks, and logging.
 *
 * An App orchestrates a set of Modules working together in a ModuleSystem, with Tasks used to
 * perform timed operations and a Logger for monitoring and debugging.
 */
class App {
 public:
  /**
   * @brief Defines the type of logger to be used by the application.
   */
  enum class LogType {
    kNone = 0,    ///< No logging enabled.
    kSerial = 1,  ///< Use Serial for logging.
    kUdp = 2      ///< Use UDP for logging (requires network setup).
  };

  /**
   * @brief Configuration options for the App.
   */
  struct Options {
    unsigned reserve_num_modules = 8;   ///< @brief Initial capacity for the module system.
    unsigned reserve_tasks = 16;        ///< @brief Initial capacity for scheduled tasks.
    LogType log_type = LogType::kNone;  ///< @brief The type of logger to use.
    String board_name;                  ///< @brief The name of the board/device.

    /**
     * @brief Sets the initial capacity for the module system.
     * @param val The number of modules to reserve space for.
     * @return Reference to this Options object for chaining.
     */
    Options& withReserveNumModules(unsigned val) {
      this->reserve_num_modules = val;
      return *this;
    }
    /**
     * @brief Sets the initial capacity for scheduled tasks.
     * @param val The number of tasks to reserve space for.
     * @return Reference to this Options object for chaining.
     */
    Options& withReserveTasks(unsigned val) {
      this->reserve_tasks = val;
      return *this;
    }
    /**
     * @brief Sets the type of logger to be used.
     * @param val The desired LogType.
     * @return Reference to this Options object for chaining.
     */
    Options& withLogType(LogType val) {
      this->log_type = val;
      return *this;
    }
  };

  /**
   * @brief Constructs an App instance with the given options.
   * @param options Configuration options for the App.
   */
  explicit App(const Options& options);

  /**
   * @brief Initializes the application by setting up the module system.
   *
   * This method should be called once in the Arduino `setup()` function.
   */
  void setup() { m_module_system.setup(); }

  /**
   * @brief Runs the main application loop, updating modules and processing tasks.
   *
   * This method should be called repeatedly in the Arduino `loop()` function.
   */
  void loop() {
    m_module_system.update();
    m_tasks.loop();
  }

  /**
   * @brief Gets a reference to the ModuleSystem.
   * @return Reference to the ModuleSystem.
   */
  ModuleSystem& module_system() { return m_module_system; }
  /**
   * @brief Gets a constant reference to the ModuleSystem.
   * @return Constant reference to the ModuleSystem.
   */
  const ModuleSystem& module_system() const { return m_module_system; }

  /**
   * @brief Gets a reference to the Tasks module for scheduling operations.
   * @return Reference to the Tasks module.
   */
  Tasks& tasks() { return m_tasks; }

  /**
   * @brief Gets a reference to the Logger used by the application.
   * @return Reference to the Logger.
   */
  Logger& log() { return *m_logger; }

  /**
   * @brief Gets the configuration options used to create this App.
   * @return Constant reference to the App's Options.
   */
  const Options& options() const { return m_options; }

  /**
   * @brief Gets a reference to the NullLogger.
   * @return Reference to the NullLogger.
   */
  NullLogger& nullLogger() { return m_null_logger; }

  /**
   * @brief Gets a reference to the SerialLogger.
   * @return Reference to the SerialLogger.
   */
  SerialLogger& serialLogger() { return m_serial_logger; }

  /**
   * @brief Gets the board name as a String.
   * @return The board name.
   */
  virtual const String& board_name() const { return m_options.board_name; }

  /**
   * @brief Gets the board name as a C-style string.
   * @return The board name.
   */
  const char* board_cname() const { return board_name().c_str(); }

 private:
  const Options m_options;       ///< @brief Configuration options for the App.
  NullLogger m_null_logger;      ///< @brief A logger that discards all messages.
  SerialLogger m_serial_logger;  ///< @brief A logger that outputs to Serial.

 protected:
  Logger* m_logger;  ///< @brief Pointer to the active logger (can be Null, Serial, or UDP).

 private:
  ModuleSystem m_module_system;  ///< @brief The core module management system.
  Tasks m_tasks;                 ///< @brief The tasks module for scheduling.
};

}  // namespace og3
