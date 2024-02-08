// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include <vector>

namespace og3 {

class VariableBase;

// A VariableGroup is a collection of variables which can be loaded together from
// a configuration file, edited together in a web form, or written together to MQTT.
class VariableGroup {
 public:
  VariableGroup(const char* name, size_t initial_size = 16);
  VariableGroup(const VariableGroup&) = delete;
  void add(VariableBase* variable);
  const char* name() const { return m_name; }

  const std::vector<VariableBase*>& variables() const { return m_variables; }
  std::vector<VariableBase*>& variables() { return m_variables; }

  unsigned num_config() const { return m_num_config; }

  void toJson(JsonDocument* out_json, unsigned flags) const;
  void toJson(String* out_str, unsigned flags) const;
  void toJson(std::ostream* out_str, unsigned flags) const;

 private:
  const char* m_name;
  unsigned m_num_config = 0;
  std::vector<VariableBase*> m_variables;
};

// A Variable of a type derived from VariableBase has a name, string represenation,
//  description, and set of flags which help it to be used as a configuration value,
//  set in a web interface, written to and read from JSON (for MQTT for flash storage), etc....
class VariableBase {
 public:
  VariableBase(const VariableBase&) = delete;
  VariableBase(const char* name_, const char* units_, const char* description_, unsigned flags_,
               VariableGroup* group);

  virtual String string() const = 0;
  virtual bool fromString(const String&) = 0;

  virtual void toJson(JsonDocument* doc) = 0;
  virtual bool fromJson(const JsonVariant& doc) = 0;

  virtual String formEntry() const;
  enum Flags {
    kSettable = 0x01,  // may be set via web form
    kConfig = 0x02,    // persist value via flash
    kNoPublish = 0x4,  // do not publish via mqtt
    kNoDisplay = 0x8,  // do not write into web tables for display
  };

  const char* name() const { return m_name; }
  const char* units() const { return m_units; }
  const char* description() const { return m_description; }
  const char* human_str() const {
    return m_description && m_description[0] ? description() : name();
  }
  const VariableGroup* group() const { return m_group; }

  unsigned flags() const { return m_flags; }
  bool settable() const { return testFlag(Flags::kSettable); }
  bool config() const { return testFlag(Flags::kConfig); }
  bool noPublish() const { return testFlag(Flags::kNoPublish); }
  bool noDisplay() const { return testFlag(Flags::kNoDisplay); }
  bool failed() const { return m_failed; }
  void setFailed(bool failed = true) { m_failed = failed; }

 private:
  bool testFlag(Flags flag) const { return m_flags & static_cast<unsigned>(flag); }

  const char* m_name;
  const char* m_units;
  const char* m_description;
  const unsigned m_flags;
  VariableGroup* m_group;
  bool m_failed = false;
};

class FloatVariableBase : public VariableBase {
 public:
  FloatVariableBase(const char* name_, const char* units_, const char* description_,
                    unsigned flags_, unsigned decimals_, VariableGroup* group)
      : VariableBase(name_, units_, description_, flags_, group), m_decimals(decimals_) {}

  unsigned decimals() const { return m_decimals; }

 private:
  const unsigned m_decimals;
};

template <typename T>
class Variable : public VariableBase {
 public:
  Variable(const char* name_, const T& value, const char* units_, const char* description_,
           unsigned flags_, VariableGroup* group)
      : VariableBase(name_, units_, description_, flags_, group), m_value(value) {}
  String string() const override;
  bool fromString(const String&) override;
  void toJson(JsonDocument* doc) override;
  bool fromJson(const JsonVariant& json) override;

  const T& value() const { return m_value; }
  T& value() { return m_value; }
  Variable<T>& operator=(const T& value) {
    m_value = value;
    setFailed(false);
    return *this;
  }

 protected:
  T m_value;
};

template <typename T>
class FloatingPointVariable : public FloatVariableBase {
 public:
  FloatingPointVariable(const char* name_, const T& value, const char* units_,
                        const char* description_, unsigned flags_, unsigned decimals_,
                        VariableGroup* group)
      : FloatVariableBase(name_, units_, description_, flags_, decimals_, group), m_value(value) {}
  String string() const override;
  bool fromString(const String&) override;
  void toJson(JsonDocument* doc) override;
  bool fromJson(const JsonVariant& json) override;

  const T& value() const { return m_value; }
  T& value() { return m_value; }
  FloatingPointVariable<T>& operator=(const T& value) {
    m_value = value;
    setFailed(false);
    return *this;
  }

 protected:
  T m_value;
};

class EnumVariableBase : public VariableBase {
 public:
  EnumVariableBase(const char* name_, const char* units_, const char* description_, unsigned flags_,
                   VariableGroup* group)
      : VariableBase(name_, units_, description_, flags_, group) {}
};

template <typename T>
class EnumVariable : public EnumVariableBase {
 public:
  EnumVariable(const char* name_, const T& value, const char* units_, const char* description_,
               unsigned flags_, VariableGroup* group)
      : EnumVariableBase(name_, units_, description_, flags_, group), m_value(value) {}
  String string() const override { return String(static_cast<int>(value())); }
  bool fromString(const String& value) override {
    int ival = -1;
    setFailed(1 != sscanf(value.c_str(), "%d", &ival));
    if (failed()) {
      return false;
    }
    m_value = static_cast<T>(ival);
    return true;
  }
  void toJson(JsonDocument* doc) override {
    if (!failed()) {
      (*doc)[name()] = static_cast<int>(value());
    }
  }
  bool fromJson(const JsonVariant& json) override {
    if (!json.is<int>()) {
      return false;
    }
    m_value = static_cast<T>(json.as<int>());
    return true;
  }

  const T& value() const { return m_value; }
  T& value() { return m_value; }
  EnumVariable<T>& operator=(const T& value) {
    m_value = value;
    setFailed(false);
    return *this;
  }

 protected:
  T m_value;
};

template <>
inline String FloatingPointVariable<float>::string() const {
  return String(m_value, decimals());
}
template <>
inline String FloatingPointVariable<double>::string() const {
  return String(m_value, decimals());
}
template <>
inline String Variable<String>::string() const {
  return m_value;
}
template <typename T>
inline String Variable<T>::string() const {
  return String(m_value);
}

template <>
inline bool FloatingPointVariable<float>::fromString(const String& value) {
  setFailed(1 != sscanf(value.c_str(), "%g", &m_value));
  return !failed();
}
template <>
inline bool FloatingPointVariable<double>::fromString(const String& value) {
  setFailed(1 != sscanf(value.c_str(), "%lg", &m_value));
  return !failed();
}
template <>
inline bool Variable<int>::fromString(const String& value) {
  setFailed(1 != sscanf(value.c_str(), "%d", &m_value));
  return !failed();
}
template <>
inline bool Variable<unsigned>::fromString(const String& value) {
  setFailed(1 != sscanf(value.c_str(), "%u", &m_value));
  return !failed();
}
template <>
inline bool Variable<String>::fromString(const String& value) {
  m_value = value;
  return true;
}
template <>
inline bool Variable<bool>::fromString(const String& value) {
  setFailed(false);
  if (value.length() == 0 || value == "0"       // 0 vs 1
      || value[0] == 'f' || value[0] == 'F'     // false vs true
      || ((value[0] == 'o' || value[0] == 'O')  // off vs on
          && (value[1] == 'f' || value[1] == 'F'))) {
    m_value = false;
  } else {
    m_value = true;
  }
  return true;
}
template <>
inline bool Variable<bool>::fromJson(const JsonVariant& json) {
  if (json.is<const char*>()) {
    return this->fromString(json.as<const char*>());
  }
  if (!json.is<bool>()) {
    setFailed();
    return false;
  }
  m_value = json.as<bool>();
  return true;
}

template <>
inline void Variable<String>::toJson(JsonDocument* doc) {
  if (!failed()) {
    (*doc)[name()] = value().c_str();
  }
}

template <typename T>
inline void Variable<T>::toJson(JsonDocument* doc) {
  if (!failed()) {
    (*doc)[name()] = value();
  }
}

template <typename T>
inline void FloatingPointVariable<T>::toJson(JsonDocument* doc) {
  if (!failed()) {
    (*doc)[name()] = value();
  }
}

template <>
inline bool Variable<String>::fromJson(const JsonVariant& json) {
  if (!json.is<const char*>()) {
    return false;
  }
  m_value = json.as<const char*>();
  return true;
}

template <typename T>
inline bool Variable<T>::fromJson(const JsonVariant& json) {
  if (!json.is<T>()) {
    return false;
  }
  m_value = json.as<T>();
  return true;
}

template <typename T>
inline bool FloatingPointVariable<T>::fromJson(const JsonVariant& json) {
  if (!json.is<T>()) {
    setFailed();
    return false;
  }
  m_value = json.as<T>();
  return true;
}

using FloatVariable = FloatingPointVariable<float>;
using DoubleVariable = FloatingPointVariable<double>;

class BoolVariable : public Variable<bool> {
 public:
  BoolVariable(const char* name_, const bool value, const char* description_, unsigned flags_,
               VariableGroup* group)
      : Variable<bool>(name_, value, "", description_, flags_, group) {}
  String string() const final { return value() ? "true" : "false"; }
  void toJson(JsonDocument* doc);
  BoolVariable& operator=(bool value);
  String formEntry() const override;
};

class BinarySensorVariable : public Variable<bool> {
 public:
  BinarySensorVariable(const char* name_, const bool value, const char* description_,
                       VariableGroup* group, bool publish = true)
      : Variable<bool>(name_, value, "", description_, publish ? 0 : kNoPublish, group) {}
  String string() const final { return value() ? "ON" : "OFF"; }
  void toJson(JsonDocument* doc);
  bool fromJson(const JsonVariant& json);
  BinarySensorVariable& operator=(bool value);
};

}  // namespace og3
