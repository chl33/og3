// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/app.h"
#include "og3/compiler_definitions.h"
#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/variable.h"

class AsyncWebServerRequest;

namespace og3 {

class MqttManager;

// A module which sends basic application status (memory available, uptime)
//  via MQTT every couple minutes.
class AppStatus : public Module {
 public:
  AppStatus(Tasks* tasks, App::LogType log_type);

  static const char* kName;
  static const char* kUrl;

  // Returns true if filesystem existed on previous boot.
  void read();

  const VariableGroup& variables() const { return m_vg; }

 private:
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
