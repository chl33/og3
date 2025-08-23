// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/variable.h"

#include <memory>

#include "og3/html_table.h"

namespace og3 {

std::string VariableBase::formEntry() const {
  std::string ret = "<input id='";
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

VariableGroup::VariableGroup(const char* name, const char* id, size_t initial_size)
    : m_name(name), m_id(id ? id : name) {
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

void VariableGroup::toJson(std::string* out_str, unsigned flags) const {
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
                           unsigned flags_, VariableGroup& group)
    : m_name(name_), m_units(units_), m_description(description_), m_flags(flags_), m_group(group) {
  group.add(this);
}

EnumStrVariableBase::EnumStrVariableBase(const char* name_, int value, const char* description_,
                                         unsigned num_values, const char* value_names[],
                                         unsigned flags_, VariableGroup& group)
    : EnumVariableBase(name_, description_, flags_, group),
      m_num_values(num_values),
      m_value_names(value_names),
      m_value(value) {}

std::string EnumStrVariableBase::string() const {
  if (m_value < 0 || m_value >= static_cast<int>(m_num_values)) {
    return "??";
  }
  return std::string(m_value_names[m_value]);
}
bool EnumStrVariableBase::fromString(const std::string& value) {
  for (unsigned i = 0; i < m_num_values; i += 1) {
    if (0 == strcmp(m_value_names[i], value.c_str())) {
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
  for (unsigned i = 0; i < m_num_values; i += 1) {
    if (0 == strcmp(m_value_names[i], json.as<const char*>())) {
      m_value = i;
      return true;
    }
  }
  return false;
}
void EnumStrVariableBase::toJson(JsonDocument* doc) {
  if (!failed()) {
    (*doc)[name()] = string();
  }
}

std::string EnumStrVariableBase::formEntry() const {
  std::string ret = "<select name=\"";
  ret += name();
  ret += "\">\n";
  // ret += "<option value=\"\">--Please choose an option--</option>\n";
  for (unsigned i = 0; i < m_num_values; i += 1) {
    ret += "<option value=\"";
    ret += i;
    ret += "\"";
    if (static_cast<int>(i) == m_value) {
      ret += " selected";
    }
    ret += ">";
    ret += m_value_names[i];
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

std::string BoolVariable::formEntry() const {
  auto radio = [this](const std::string& val, bool checked) -> std::string {
    std::string id_val;
    html::escape(&id_val, name());
    id_val += "_" + val;
    return "<input id='" + id_val + "' name='" + name() + "' type='radio' value='" + val + "'" +
           (checked ? " checked" : "") + "><label for='" + id_val + "'>" + val + "</label>";
  };
  return std::string("<p>") + human_str() + radio("true", value()) + "\n" +
         radio("false", !value()) + "</p>\n";
}

BinaryCoverSensorVariable& BinaryCoverSensorVariable::operator=(bool value) {
  m_value = value;
  setFailed(false);
  return *this;
}

}  // namespace og3
