// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/logger.h"
#include "og3/module_system.h"
#include "og3/serial_logger.h"
#include "og3/tasks.h"

namespace og3 {

// An App consists of a set of Modules working together in a ModuleSystem, with Tasks used to
// perform timed tasks.
class App {
 public:
  enum class LogType { kNone = 0, kSerial = 1, kUdp = 2 };

  struct Options {
    unsigned reserve_num_modules = 8;
    unsigned reserve_tasks = 16;
    LogType log_type = LogType::kNone;

    Options& withReserveNumModules(unsigned val) {
      this->reserve_num_modules = val;
      return *this;
    }
    Options& withReserveTasks(unsigned val) {
      this->reserve_tasks = val;
      return *this;
    }
    Options& withLogType(LogType val) {
      this->log_type = val;
      return *this;
    }
  };

  explicit App(const Options& options);

  void setup() { m_module_system.setup(); }
  void loop() {
    m_module_system.update();
    m_tasks.loop();
  }

  ModuleSystem& module_system() { return m_module_system; }
  const ModuleSystem& module_system() const { return m_module_system; }
  Tasks& tasks() { return m_tasks; }
  Logger& log() { return *m_logger; }
  const Options& options() const { return m_options; }
  NullLogger& nullLogger() { return m_null_logger; }
  SerialLogger& serialLogger() { return m_serial_logger; }

 private:
  const Options m_options;
  NullLogger m_null_logger;
  SerialLogger m_serial_logger;

 protected:
  Logger* m_logger;

 private:
  ModuleSystem m_module_system;
  Tasks m_tasks;
};

}  // namespace og3
