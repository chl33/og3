// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <cstring>
#include <functional>
#include <limits>
#include <string>
#include <unordered_map>

namespace og3 {

class Module;

/** @brief Type for simple parameterless callbacks. */
using Thunk = std::function<void()>;

/** @brief Map for retrieving modules by their unique names. */
using NameToModule = std::unordered_map<std::string, Module*>;

/**
 * @brief Checks if timestamp t1 is chronologically before t2, handling 32-bit overflow.
 * @param t1 The earlier timestamp (millis).
 * @param t2 The later timestamp (millis).
 * @return true if t1 is before t2.
 */
inline bool isBefore(unsigned long t1, unsigned long t2) {
  const unsigned long d = t2 - t1;
  return d < std::numeric_limits<unsigned long>::max() / 2;
}

}  // namespace og3
