// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>

#include "og3/variable.h"

namespace og3::html {

void escape(std::string* out_str, const char* in_str);

void writeRowInto(std::string* out_str, const VariableBase& var, const char* name = nullptr);

void writeRowsInto(std::string* out_str, const VariableGroup& vars);
void writeTableInto(std::string* out_str, const VariableGroup& vars, const char* title = nullptr,
                    const char* css_class = nullptr);
void writeTableStart(std::string* out_str, const char* title, const char* css_class = nullptr);
void writeTableEnd(std::string* out_str);

void writeFormTableInto(std::string* out_str, const VariableGroup& vars,
                        const char* title = nullptr, const char* css_class = nullptr);

}  // namespace og3::html
