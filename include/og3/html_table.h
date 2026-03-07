// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>

#include "og3/variable.h"

/**
 * @brief Utilities for generating HTML tables from Variable objects.
 */
namespace og3::html {

/**
 * @brief Escapes HTML special characters in a string.
 * @param out_str Pointer to the output String.
 * @param in_str The raw input string.
 */
void escape(String* out_str, const char* in_str);

/**
 * @brief Writes a single table row for a variable.
 * @param out_str The output HTML string.
 * @param var The variable to render.
 * @param name Optional override for the row label.
 */
void writeRowInto(String* out_str, const VariableBase& var, const char* name = nullptr);

/**
 * @brief Writes multiple table rows from a VariableGroup.
 * @param out_str The output HTML string.
 * @param vars The group of variables.
 */
void writeRowsInto(String* out_str, const VariableGroup& vars);

/**
 * @brief Writes a complete HTML table for a VariableGroup.
 * @param out_str The output HTML string.
 * @param vars The group of variables.
 * @param title Optional table title.
 * @param css_class Optional CSS class for the <table> tag.
 */
void writeTableInto(String* out_str, const VariableGroup& vars, const char* title = nullptr,
                    const char* css_class = nullptr);

/**
 * @brief Writes the opening tags for an HTML table.
 * @param out_str The output HTML string.
 * @param title The table title.
 * @param css_class Optional CSS class.
 */
void writeTableStart(String* out_str, const char* title, const char* css_class = nullptr);

/**
 * @brief Writes the closing tags for an HTML table.
 * @param out_str The output HTML string.
 */
void writeTableEnd(String* out_str);

/**
 * @brief Writes a complete HTML form for editing a VariableGroup.
 * @param out_str The output HTML string.
 * @param vars The group of variables.
 * @param title Optional form title.
 * @param css_class Optional CSS class.
 */
void writeFormTableInto(String* out_str, const VariableGroup& vars, const char* title = nullptr,
                        const char* css_class = nullptr);

}  // namespace og3::html
