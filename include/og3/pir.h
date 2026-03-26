// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/din.h"
#include "og3/module.h"
#include "og3/tasks.h"
#include "og3/util.h"
#include "og3/variable.h"

namespace og3 {

class HADiscovery;

/**
 * @brief Module for managing Passive Infrared (PIR) motion sensors.
 *
 * Pir handles reading from a digital input pin connected to a PIR sensor.
 * It supports both polling and interrupt-driven motion detection, with
 * Home Assistant discovery support.
 */
class Pir : public Module {
 public:
  /**
   * @brief Constructs a Pir module.
   * @param module_name Unique name for the module.
   * @param motion_name Name for the motion state variable.
   * @param module_system The ModuleSystem to register with.
   * @param pin The hardware GPIO pin.
   * @param description Human-readable description.
   * @param vg The VariableGroup to add the state variable to.
   * @param publish true to publish via MQTT.
   * @param ha_discovery true to enable Home Assistant discovery.
   */
  Pir(const char* module_name, const char* motion_name, ModuleSystem* module_system, uint8_t pin,
      const char* description, VariableGroup& vg, bool publish, bool ha_discovery);

  /** @brief Polls the sensor state. */
  void read();

  /** @return true if motion was detected in the last read. */
  bool motion() const { return m_din.isHigh(); }

  /**
   * @brief Registers a callback to be executed when motion is detected.
   *
   * This sets up a hardware interrupt. The callback is scheduled via the
   * Tasks system to avoid running complex logic inside the ISR.
   * @param fn The callback function.
   */
  void callOnMotion(const Thunk& fn);

 private:
  bool haDeclare(HADiscovery* had, JsonObject json);

#ifndef NATIVE
  static void IRAM_ATTR _onMotion();
#endif

  static bool s_interrupt_setup;

  HADiscovery* m_ha_discovery = nullptr;
  DIn m_din;
};

}  // namespace og3
