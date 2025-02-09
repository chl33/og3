// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <functional>
#include <limits>
#include <map>

namespace og3 {

class Module;

using Thunk = std::function<void()>;
using NameToModule = std::map<const char*, Module*>;
using LinkFn = std::function<bool(NameToModule&)>;

// Return true if t1 is before t2, allowing that t1 may have wrapped around.
inline bool isBefore(unsigned long t1, unsigned long t2) {
  const unsigned long d = t2 - t1;
  return d < std::numeric_limits<unsigned long>::max() / 2;
}

}  // namespace og3
