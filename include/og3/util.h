// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <functional>
#include <map>

namespace og3 {

class Module;

using Thunk = std::function<void()>;
using NameToModule = std::map<const char*, Module*>;
using LinkFn = std::function<bool(NameToModule&)>;

}  // namespace og3
