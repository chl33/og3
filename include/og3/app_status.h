// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/app.h"
#include "og3/compiler_definitions.h"
#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/variable.h"

class NetRequest;

namespace og3 {

class MqttManager;

/**
 * @brief A module which sends basic application status (memory available, uptime)
 * via MQTT every couple minutes.
 */
class AppStatus : public Module {
 public:
  /**
   * @brief Construct a new AppStatus object.
   * @param tasks The tasks instance for scheduling status updates.
   * @param log_type The application log type.
   */
  AppStatus(Tasks* tasks, App::LogType log_type);

  static const char* kName;
  static const char* kUrl;

  /**
   * @brief Read the current application status.
   */
  void read();

  /**
   * @brief Get the variable group containing status variables.
   * @return The variable group.
   */
  const VariableGroup& variables() const { return m_vg; }

 private:
  /**
   * @brief Send the current status via MQTT.
   */
  void mqttSend();

  Tasks* const m_tasks;
  VariableGroup m_vg;
  FloatVariable m_mem_available;
  Variable<unsigned> m_uptime_msec;
  Variable<unsigned> m_num_tasks;
  Variable<unsigned> m_task_capacity;
  Variable<unsigned> m_num_modules;
  Variable<unsigned> m_module_capacity;
  EnumStrVariable<App::LogType> m_log_type;
  MqttManager* m_mqtt_manager = nullptr;
};

}  // namespace og3
