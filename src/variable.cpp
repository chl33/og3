// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/variable.h"

#include "og3/html_table.h"

namespace og3 {

String VariableBase::formEntry() const {
  String ret = "<input id='";
  ret += name();
  ret += "' name='";
  ret += name();
  ret +=
      "' autocomplete='off' autocorrect='off' autocapitalize='off' spellcheck='false'\n"
      "  placeholder=' ' value='";
  html::escape(&ret, string().c_str());
  ret += "'>\n";
  return ret;
}

VariableGroup::VariableGroup(const char* name, size_t initial_size) : m_name(name) {
  m_variables.reserve(initial_size);
}

void VariableGroup::add(VariableBase* variable) {
  m_variables.push_back(variable);
  if (variable->config()) {
    m_num_config += 1;
  }
}

void VariableGroup::toJson(JsonDocument* out_json, unsigned flags) const {
  for (auto* var : variables()) {
    if (flags & var->flags() & VariableBase::kNoPublish) {
      continue;
    }
    if (var->config() && !(flags & VariableBase::kConfig)) {
      continue;
    }
    var->toJson(out_json);
  }
}

void VariableGroup::toJson(String* out_str, unsigned flags) const {
#ifndef NATIVE
  JsonDocument doc;
  toJson(&doc, flags);
  serializeJson(doc, *out_str);
#endif
}

void VariableGroup::toJson(std::ostream* out_str, unsigned flags) const {
  JsonDocument doc;
  toJson(&doc, flags);
  serializeJson(doc, *out_str);
}

VariableBase::VariableBase(const char* name_, const char* units_, const char* description_,
                           unsigned flags_, VariableGroup* group)
    : m_name(name_), m_units(units_), m_description(description_), m_flags(flags_), m_group(group) {
  if (m_group) {
    m_group->add(this);
  }
}

EnumStrVariableBase::EnumStrVariableBase(const char* name_, int value, const char* units_,
                                         const char* description_, int min_value, int max_value,
                                         const char* value_names[], unsigned flags_,
                                         VariableGroup* group)
    : EnumVariableBase(name_, units_, description_, flags_, group),
      m_min_value(min_value),
      m_max_value(max_value),
      m_value_names(value_names),
      m_value(value) {}

String EnumStrVariableBase::string() const {
  if (m_value < m_min_value || m_value > m_max_value) {
    return "??";
  }
  return String(m_value_names[m_value - m_min_value]);
}
bool EnumStrVariableBase::fromString(const String& value) {
  for (int i = m_min_value; i <= m_max_value; i += 1) {
    if (0 == strcmp(m_value_names[i - m_min_value], value.c_str())) {
      m_value = i;
      setFailed(false);
      return true;
    }
  }
  setFailed(1 != sscanf(value.c_str(), "%d", &m_value));
  return !failed();
}
bool EnumStrVariableBase::fromJson(const JsonVariant& json) {
  if (json.is<int>()) {
    m_value = json.as<int>();
    return true;
  }
  if (!json.is<const char*>()) {
    return false;
  }
  for (int i = m_min_value; i <= m_max_value; i += 1) {
    if (0 == strcmp(m_value_names[i - m_min_value], json.as<const char*>())) {
      m_value = i;
      return true;
    }
  }
  return false;
}
void EnumStrVariableBase::toJson(JsonDocument* doc) {
  if (!failed()) {
    (*doc)[name()] = m_value;
  }
}

String EnumStrVariableBase::formEntry() const {
  String ret = "<select name=\"";
  ret += name();
  ret += "\">\n";
  // ret += "<option value=\"\">--Please choose an option--</option>\n";
  for (int i = m_min_value; i <= m_max_value; i += 1) {
    ret += "<option value=\"";
    ret += i;
    ret += "\"";
    if (i == m_value) {
      ret += " selected";
    }
    ret += ">";
    ret += m_value_names[i - m_min_value];
    ret += "</option>\n";
  }
  ret += "</select>\n";
  return ret;
}

void BoolVariable::toJson(JsonDocument* doc) {
  if (!failed()) {
#ifndef NATIVE
    (*doc)[name()] = string();
#else
    (*doc)[name()] = string().c_str();
#endif
  }
}

BoolVariable& BoolVariable::operator=(bool value) {
  m_value = value;
  setFailed(false);
  return *this;
}

void BinarySensorVariable::toJson(JsonDocument* doc) {
  if (!failed()) {
#ifndef NATIVE
    (*doc)[name()] = string();
#else
    (*doc)[name()] = string().c_str();
#endif
  }
}

bool BinarySensorVariable::fromJson(const JsonVariant& json) {
  if (json.is<const char*>()) {
    m_value = fromString(json.as<const char*>());
    return true;
  }
  if (json.is<const char*>()) {
    m_value = json.as<const char*>();
    return true;
  }
  return false;
}

BinarySensorVariable& BinarySensorVariable::operator=(bool value) {
  m_value = value;
  setFailed(false);
  return *this;
}

String BoolVariable::formEntry() const {
  auto radio = [this](const String& val, bool checked) -> String {
    String id_val;
    html::escape(&id_val, name());
    id_val += "_" + val;
    return "<input id='" + id_val + "' name='" + name() + "' type='radio' value='" + val + "'" +
           (checked ? " checked" : "") + "><label for='" + id_val + "'>" + val + "</label>";
  };
  return String("<p>") + human_str() + radio("true", value()) + "\n" + radio("false", !value()) +
         "</p>\n";
}

}  // namespace og3
