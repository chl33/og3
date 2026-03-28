// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/module.h"

namespace og3 {

/**
 * @brief Module for managing filesystem initialization in flash storage.
 *
 * FlashSupport handles the setup of the LittleFS filesystem and provides
 * information about whether the filesystem pre-existed before the current boot.
 */
class FlashSupport : public Module {
 public:
  /** @brief Constructs a FlashSupport module. */
  explicit FlashSupport(ModuleSystem* module_system);

  static const char* kName;  ///< @brief "flash"

  /**
   * @brief Initializes the LittleFS filesystem.
   * @return true if the filesystem already existed on previous boot.
   */
  bool setup();

  /** @return true if setup() has been successfully called. */
  bool isSetup() const { return m_is_setup; }
  /** @return true if the filesystem existed before this boot. */
  bool fsPreexisted() const { return m_fs_preexisted; }

 private:
  bool m_is_setup = false;
  bool m_fs_preexisted = false;
};

}  // namespace og3
