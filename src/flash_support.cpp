// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/flash_support.h"

#if defined(USE_SPIFFS)
#include <SPIFFS.h>
#elif defined(NATIVE)
#else
#include <LittleFS.h>
#endif

namespace og3 {

const char* FlashSupport::kName = "flash";

FlashSupport::FlashSupport(ModuleSystem* module_system)
    : Module(FlashSupport::kName, module_system) {}

bool FlashSupport::setup() {
  if (m_is_setup) {
    return fsPreexisted();
  }
#if defined(USE_SPIFFS)
  if (SPIFFS.begin()) {
    m_fs_preexisted = true;
  } else {
    SPIFFS.format();
    m_fs_preexisted = false;
  }
#elif defined(NATIVE)
  m_fs_preexisted = true;
#else
  if (LittleFS.begin()) {
    m_fs_preexisted = true;
  } else {
    LittleFS.format();
    m_fs_preexisted = false;
  }
#endif
  m_is_setup = true;
  return fsPreexisted();
}

}  // namespace og3
