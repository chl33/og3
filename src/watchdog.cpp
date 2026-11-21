// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/watchdog.h"

#include <chrono>

#ifndef NATIVE
#ifdef ARDUINO_ARCH_ESP32
#include <esp_idf_version.h>
#include <esp_task_wdt.h>
#endif
#endif

namespace og3 {

Watchdog::Watchdog(App* app, std::chrono::seconds timeout, std::chrono::milliseconds update)
    : Module("watchdog", &app->module_system()), m_app(app), m_update(update) {
#ifndef NATIVE
#ifdef ARDUINO_ARCH_ESP32
  add_init_fn([timeout, this]() {
  // Initialize Watchdog Timer
#if ESP_IDF_VERSION_MAJOR >= 5
    uint32_t idle_core_mask = 0;
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
    idle_core_mask |= (1 << 0);
#endif
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
    idle_core_mask |= (1 << 1);
#endif
    const esp_task_wdt_config_t wdt_config = {
        .timeout_ms = static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()),
        .idle_core_mask = idle_core_mask,
        .trigger_panic = true,
    };
    if (esp_task_wdt_init(&wdt_config) == ESP_ERR_INVALID_STATE) {
      esp_task_wdt_reconfigure(&wdt_config);
    }
#else
    esp_task_wdt_init(timeout.count(), true);
#endif
    esp_task_wdt_add(NULL);  // Add current thread (loopTask) to WDT
  });
  add_start_fn([this]() { this->update(); });
#endif
#endif
}

void Watchdog::enable() {
#ifndef NATIVE
#ifdef ARDUINO_ARCH_ESP32
  esp_task_wdt_add(NULL);
#endif
#endif
}

void Watchdog::disable() {
#ifndef NATIVE
#ifdef ARDUINO_ARCH_ESP32
  esp_task_wdt_delete(NULL);
#endif
#endif
}

void Watchdog::update() {
#ifndef NATIVE
#ifdef ARDUINO_ARCH_ESP32
  esp_task_wdt_reset();  // Reset watchdog timer
  // Update again in one second.
  m_app->tasks().runIn(m_update.count(), [this]() { update(); });
#endif
#endif
}

}  // namespace og3
