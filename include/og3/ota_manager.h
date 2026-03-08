// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/module.h"
#include "og3/wifi_manager.h"

namespace og3 {

/**
 * @brief Module providing Over-The-Air (OTA) firmware update support.
 *
 * OtaManager configures the platform-specific OTA system (e.g. ArduinoOTA)
 * and provides callbacks for monitoring update progress and status.
 */
class OtaManager : public Module {
 public:
  static const char kName[];  ///< @brief "ota"

  /** @brief Configuration options for OTA. */
  struct Options {
    Options(const char* default_password_ = nullptr) : default_password(default_password_) {}
    const char* default_password;  ///< Optional password for OTA authentication.
  };

  /** @brief Constructs an OtaManager. */
  OtaManager(const Options& opts, ModuleSystem* module_system);

  /** @brief Registers a callback for when an update starts. */
  void addOtaStartCallback(const std::function<void()>& callback) {
    m_startCallbacks.push_back(callback);
  }
  /** @brief Registers a callback for when an update successfully ends. */
  void addOtaEndCallback(const std::function<void()>& callback) {
    m_endCallbacks.push_back(callback);
  }
  /** @brief Registers a callback for when an update fails. */
  void addOtaErrorCallback(const std::function<void(int)>& callback) {
    m_errorCallbacks.push_back(callback);
  }

  /** @return Pointer to the OtaManager module instance. */
  static OtaManager* get(const NameToModule& n2m) { return GetModule<OtaManager>(n2m, kName); }

 private:
  void setup();
  void enable();

  const Options m_opts;
  VariableGroup m_vg;
  Variable<String> m_password;
  ConfigInterface* m_config = nullptr;
  WifiManager* m_wifi_manager = nullptr;

  std::vector<std::function<void()>> m_startCallbacks;
  std::vector<std::function<void()>> m_endCallbacks;
  std::vector<std::function<void(int)>> m_errorCallbacks;

  unsigned long m_last_progress_msec = 0;
};

}  // namespace og3
