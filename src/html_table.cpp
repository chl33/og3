// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/html_table.h"

#include <Arduino.h>

#include "og3/variable.h"

namespace og3::html {
void escape(String* out_str, const char* in_str) {
  if (!in_str) {
    return;
  }
  while (true) {
    switch (*in_str) {
      case '\0':
        return;
      case '&':
        *out_str += "&amp;";
        break;
      case '<':
        *out_str += "&lt;";
        break;
      case '>':
        *out_str += "&gt;";
        break;
      case '%':
        *out_str += "&percnt;";
        break;
      default:
        *out_str += *in_str;
        break;
    }
    in_str += 1;
  }
}

void writeRowInto(String* out_str, const VariableBase& var) {
  *out_str += "<tr><td>";
  escape(out_str, var.human_str());
  *out_str += "</td><td>";
  escape(out_str, var.string().c_str());
  *out_str += " ";
  escape(out_str, var.units());
  *out_str += "</td></tr>\n";
}

void writeRowsInto(String* out_str, const VariableGroup& vars) {
  for (const VariableBase* var : vars.variables()) {
    if (!var->noDisplay() && !var->failed()) {
      writeRowInto(out_str, *var);
    }
  }
}
void writeTableInto(String* out_str, const VariableGroup& vars, const char* title,
                    const char* css_class) {
  *out_str += "<table class=\"";
  *out_str += css_class ? css_class : "readings";
  *out_str +=
      "\">\n"
      "<thead><tr><th colspan=\"2\">";
  escape(out_str, title ? title : vars.name());
  *out_str +=
      "</th></tr></thead>\n"
      "<tbody>\n";
  writeRowsInto(out_str, vars);
  *out_str +=
      "</tbody>\n"
      "</table>\n";
}

void writeFormRowInto(String* out_str, const VariableBase& var) {
  *out_str += "<tr><td>";
  *out_str += var.human_str();
  *out_str += "</td><td>";
  var.writeHtmlFieldInto(out_str);
  *out_str += "</td></tr>\n";
}

void writeFormRowsInto(String* out_str, const VariableGroup& vars) {
  for (const VariableBase* var : vars.variables()) {
    if (var->settable()) {
      writeFormRowInto(out_str, *var);
    }
  }
}
void writeFormTableInto(String* out_str, const VariableGroup& vars, const char* title,
                        const char* css_class) {
  *out_str += "<table class=\"";
  *out_str += css_class ? css_class : "readings";
  *out_str +=
      "\">\n"
      "<thead><tr><th colspan=\"2\">";
  escape(out_str, title ? title : vars.name());
  *out_str +=
      "</th></tr></thead>\n"
      "<tbody>\n"
      "<form method='post'>\n";
  writeFormRowsInto(out_str, vars);
  *out_str +=
      "<tr><td colspan=\"2\"><button name='save' type='submit' class='button "
      "bgrn'>Save</button></td></tr>\n";
  *out_str += " </form>\n";
  *out_str +=
      "</tbody>\n"
      "</table>\n";
}

}  // namespace og3::html
