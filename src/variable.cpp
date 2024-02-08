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
    html::escape(&id_val, string().c_str());
    id_val += "_" + val;
    return "<input id='" + id_val + "' name='" + name() + "' type='radio' value='" + val + "'" +
           (checked ? " checked" : "") + "><label for='" + id_val + "'>" + val + "</label>";
  };
  return String("<p>") + human_str() + radio("true", value()) + "\n" + radio("false", !value()) +
         "</p>\n";
}

}  // namespace og3
