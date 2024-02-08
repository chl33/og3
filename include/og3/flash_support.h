// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "og3/compiler_definitions.h"
#include "og3/module.h"

namespace og3 {

// Class which sets-up a filesystem in flash storage, and reports whether it had
//  already been setup previously (in a previous boot).
class FlashSupport : public Module {
 public:
  explicit FlashSupport(ModuleSystem* module_system) RT3_NONNULL();

  static const char* kName;

  // Returns true if filesystem existed on previous boot.
  bool setup();
  bool isSetup() const { return m_is_setup; }
  bool fsPreexisted() const { return m_fs_preexisted; }

  static FlashSupport* get(const NameToModule& n2m) { return GetModule<FlashSupport>(n2m, kName); }

 private:
  bool m_is_setup = false;
  bool m_fs_preexisted = false;
};

}  // namespace og3
