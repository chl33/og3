// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/watchdog.h"

#include <chrono>

#ifndef NATIVE
#include <esp_task_wdt.h>
#endif

namespace og3 {

Watchdog::Watchdog(App* app, std::chrono::seconds timeout, std::chrono::milliseconds update)
    : Module("watchdog", &app->module_system()), m_app(app), m_update(update) {
#ifndef NATIVE
  add_init_fn([timeout, this]() {
    // Initialize Watchdog Timer
    esp_task_wdt_init(timeout.count(), true);
    esp_task_wdt_add(NULL);  // Add current thread (loopTask) to WDT
  });
  add_start_fn([this]() { this->update(); });
#endif
}

void Watchdog::enable() {
#ifndef NATIVE
  esp_task_wdt_add(NULL);
#endif
}

void Watchdog::disable() {
#ifndef NATIVE
  esp_task_wdt_delete(NULL);
#endif
}

void Watchdog::update() {
#ifndef NATIVE
  esp_task_wdt_reset();  // Reset watchdog timer
  // Update again in one second.
  m_app->tasks().runIn(m_update.count(), [this]() { update(); });
#endif
}

}  // namespace og3
