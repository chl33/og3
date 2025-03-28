// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/app_status.h"

#include <Arduino.h>

#include "og3/html_table.h"
#include "og3/module_system.h"
#include "og3/mqtt_manager.h"
#include "og3/units.h"
#include "og3/web.h"

namespace og3 {

const char* AppStatus::kName = "app_status";
const char* AppStatus::kUrl = "/app_status";

namespace {
const char* kLogTypeNames[]{"None", "Serial", "Udp"};
}

AppStatus::AppStatus(Tasks* tasks, App::LogType log_type)
    : Module(kName, tasks->module_system()),
      m_tasks(tasks),
      m_vg(kName),
      m_mem_available("mem_avail", 0.0f, units::kKilobytes, "memory available", 0, 1, m_vg),
      m_uptime_msec("uptime", 0, units::kMilliseconds, "uptime", 0, m_vg),
      m_num_tasks("num_tasks", 0, "", "num tasks", 0, m_vg),
      m_task_capacity("task_capacity", 0, "", "task capacity", 0, m_vg),
      m_num_modules("num_modules", 0, "", "num modules", 0, m_vg),
      m_module_capacity("module_capacity", 0, "", "module capacity", 0, m_vg),
      m_log_type("log_type", log_type, "log type", App::LogType::kUdp, kLogTypeNames, 0, m_vg) {
  add_link_fn([this](const NameToModule& name_to_module) -> bool {
    m_mqtt_manager = MqttManager::get(name_to_module);
    return true;
  });
  add_start_fn([this]() {
    m_tasks->runIn(1, [this]() { read(); });
    m_tasks->runIn(20 * kMsecInSec, [this]() { mqttSend(); });
  });
}

void AppStatus::read() {
  m_uptime_msec = millis();
#ifndef NATIVE
  m_mem_available = ESP.getFreeHeap() / 1024;
#endif
  m_num_tasks = m_tasks->size();
  m_task_capacity = m_tasks->capacity();
  m_num_modules = m_tasks->module_system()->num_modules();
  m_module_capacity = m_tasks->module_system()->module_capacity();
  m_tasks->runIn(2 * kMsecInSec, [this]() { read(); });
}

void AppStatus::mqttSend() {
  if (m_mqtt_manager) {
    m_mqtt_manager->mqttSend(m_vg);
  }
  m_tasks->runIn(10 * kMsecInMin, [this]() { mqttSend(); });
}

}  // namespace og3
