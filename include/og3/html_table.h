// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>

#include "og3/variable.h"

namespace og3::html {

void escape(String* out_str, const char* in_str);

void writeRowInto(String* out_str, const VariableBase& var, const char* name = nullptr);

void writeRowsInto(String* out_str, const VariableGroup& vars);
void writeTableInto(String* out_str, const VariableGroup& vars, const char* title = nullptr,
                    const char* css_class = nullptr);
void writeTableStart(String* out_str, const char* title, const char* css_class = nullptr);
void writeTableEnd(String* out_str);

void writeFormTableInto(String* out_str, const VariableGroup& vars, const char* title = nullptr,
                        const char* css_class = nullptr);

}  // namespace og3::html
