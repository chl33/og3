// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include <cstring>
#include <vector>

namespace og3 {

class VariableBase;

/**
 * @brief A collection of variables that can be managed together.
 *
 * A VariableGroup allows grouped operations on variables, such as loading/saving
 * from configuration files, editing via web forms, or publishing to MQTT.
 */
class VariableGroup {
 public:
  /**
   * @brief Constructs a VariableGroup.
   * @param name The human-readable name of the group.
   * @param id An optional unique identifier for the group (defaults to name).
   * @param initial_size Initial capacity for the internal variables vector.
   */
  VariableGroup(const char* name, const char* id = nullptr, size_t initial_size = 16);
  VariableGroup(const VariableGroup&) = delete;

  /**
   * @brief Adds a variable to this group. Called by VariableBase constructor.
   * @param variable Pointer to the variable to add.
   */
  void add(VariableBase* variable);

  /** @return The human-readable name of the group. */
  const char* name() const { return m_name; }
  /** @return The unique identifier of the group. */
  const char* id() const { return m_id; }

  /** @return Constant reference to the list of variables in this group. */
  const std::vector<VariableBase*>& variables() const { return m_variables; }
  /** @return Reference to the list of variables in this group. */
  std::vector<VariableBase*>& variables() { return m_variables; }

  /** @return The number of variables in this group marked with the kConfig flag. */
  unsigned num_config() const { return m_num_config; }

  /**
   * @brief Serializes variables in the group to a JSON object.
   * @param out_json The target JSON object.
   * @param flags Filter flags (e.g., VariableBase::kConfig).
   */
  void toJson(JsonObject out_json, unsigned flags) const;

  /**
   * @brief Serializes the group to a JSON string.
   * @param out_str Pointer to the target String.
   * @param flags Filter flags.
   */
  void toJson(String* out_str, unsigned flags) const;

  /**
   * @brief Serializes the group to an output stream.
   * @param out_str Pointer to the target stream.
   * @param flags Filter flags.
   */
  void toJson(std::ostream* out_str, unsigned flags) const;

  /**
   * @brief Updates settable variables in the group from a JSON object.
   * @param obj The source JSON object.
   * @return The number of variables successfully updated.
   */
  unsigned updateFromJson(JsonObjectConst obj);

 private:
  const char* m_name;
  const char* m_id;
  unsigned m_num_config = 0;
  std::vector<VariableBase*> m_variables;
};

/**
 * @brief Abstract base class for all variables in the framework.
 *
 * Provides core metadata (name, units, description) and defines the interface
 * for string conversion, JSON serialization, and web form rendering.
 */
class VariableBase {
 public:
  VariableBase(const VariableBase&) = delete;

  /**
   * @brief Constructs a VariableBase and registers it with a VariableGroup.
   * @param name_ Unique name within the group.
   * @param units_ Units of measurement (can be nullptr).
   * @param description_ Human-readable description (can be nullptr).
   * @param flags_ Behavioral flags (see VariableBase::Flags).
   * @param group The VariableGroup this variable belongs to.
   */
  VariableBase(const char* name_, const char* units_, const char* description_, unsigned flags_,
               VariableGroup& group);

  virtual ~VariableBase() = default;

  /** @return String representation of the current value. */
  virtual String string() const = 0;
  /**
   * @brief Sets the value from a string.
   * @param val The source string.
   * @return true if conversion was successful.
   */
  virtual bool fromString(const String& val) = 0;

  /**
   * @brief Adds the variable's value to a JSON object.
   * @param doc The target JSON object.
   */
  virtual void toJson(JsonObject doc) = 0;
  /**
   * @brief Updates the variable's value from a JSON variant.
   * @param val The source JSON variant.
   * @return true if update was successful.
   */
  virtual bool fromJson(JsonVariantConst val) = 0;

  /** @return HTML snippet for an input field in a web form. */
  virtual String formEntry() const;

  /** @brief Flags that define variable behavior. */
  enum Flags {
    kSettable = 0x01,  ///< May be set via a web form.
    kConfig = 0x02,    ///< Persistent value saved to flash.
    kNoPublish = 0x4,  ///< Do not publish this variable via MQTT.
    kNoDisplay = 0x8,  ///< Do not include this variable in web display tables.
  };

  /** @return The unique name of the variable. */
  const char* name() const { return m_name; }
  /** @return The units of measurement. */
  const char* units() const { return m_units; }
  /** @return The human-readable description. */
  const char* description() const { return m_description; }
  /** @return Description if available, otherwise the name. */
  const char* human_str() const {
    return m_description && m_description[0] ? description() : name();
  }
  /** @return Reference to the owning VariableGroup. */
  const VariableGroup& group() const { return m_group; }

  /** @return Current behavioral flags. */
  unsigned flags() const { return m_flags; }
  /** @return true if the kSettable flag is set. */
  bool settable() const { return testFlag(Flags::kSettable); }
  /** @return true if the kConfig flag is set. */
  bool config() const { return testFlag(Flags::kConfig); }
  /** @return true if the kNoPublish flag is set. */
  bool noPublish() const { return testFlag(Flags::kNoPublish); }
  /** @return true if the kNoDisplay flag is set. */
  bool noDisplay() const { return testFlag(Flags::kNoDisplay); }
  /** @return true if the variable is in a failed/invalid state. */
  bool failed() const { return m_failed; }
  /** @brief Marks the variable as failed or healthy. */
  void setFailed(bool failed = true) { m_failed = failed; }

 private:
  bool testFlag(Flags flag) const { return m_flags & static_cast<unsigned>(flag); }

  const char* m_name;
  const char* m_units;
  const char* m_description;
  const unsigned m_flags;
  const VariableGroup& m_group;
  bool m_failed = false;
};

/**
 * @brief Base class for floating-point variables.
 */
class FloatVariableBase : public VariableBase {
 public:
  FloatVariableBase(const char* name_, const char* units_, const char* description_,
                    unsigned flags_, unsigned decimals_, VariableGroup& group)
      : VariableBase(name_, units_, description_, flags_, group), m_decimals(decimals_) {}

  /** @return Number of decimal places to use for display/serialization. */
  unsigned decimals() const { return m_decimals; }

 private:
  const unsigned m_decimals;
};

/**
 * @brief Template class for simple type variables (int, bool, String, etc.).
 */
template <typename T>
class Variable : public VariableBase {
 public:
  Variable(const char* name_, const T& value, const char* units_, const char* description_,
           unsigned flags_, VariableGroup& group)
      : VariableBase(name_, units_, description_, flags_, group), m_value(value) {}
  String string() const override;
  bool fromString(const String&) override;
  void toJson(JsonObject doc) override;
  bool fromJson(JsonVariantConst json) override;

  /** @return Constant reference to the underlying value. */
  const T& value() const { return m_value; }
  /** @return Reference to the underlying value. */
  T& value() { return m_value; }
  /** @brief Assignment operator that also clears the failed state. */
  Variable<T>& operator=(const T& value) {
    m_value = value;
    setFailed(false);
    return *this;
  }

 protected:
  T m_value;
};

/**
 * @brief Template class for floating-point variables (float, double).
 */
template <typename T>
class FloatingPointVariable : public FloatVariableBase {
 public:
  FloatingPointVariable(const char* name_, const T& value, const char* units_,
                        const char* description_, unsigned flags_, unsigned decimals_,
                        VariableGroup& group)
      : FloatVariableBase(name_, units_, description_, flags_, decimals_, group), m_value(value) {}
  String string() const override;
  bool fromString(const String&) override;
  void toJson(JsonObject doc) override;
  bool fromJson(JsonVariantConst json) override;

  /** @return Constant reference to the underlying value. */
  const T& value() const { return m_value; }
  /** @return Reference to the underlying value. */
  T& value() { return m_value; }
  /** @brief Assignment operator that also clears the failed state. */
  FloatingPointVariable<T>& operator=(const T& value) {
    m_value = value;
    setFailed(false);
    return *this;
  }

 protected:
  T m_value;
};

/**
 * @brief Base class for enumeration variables.
 */
class EnumVariableBase : public VariableBase {
 public:
  EnumVariableBase(const char* name_, const char* description_, unsigned flags_,
                   VariableGroup& group)
      : VariableBase(name_, nullptr /*units_*/, description_, flags_, group) {}
};

/**
 * @brief Base class for enumeration variables that use a string list for display.
 */
class EnumStrVariableBase : public EnumVariableBase {
 public:
  EnumStrVariableBase(const char* name_, int value, const char* description_, unsigned num_values,
                      const char* value_names[], unsigned flags_, VariableGroup& group);

  String string() const override;
  bool fromString(const String& value) override;
  bool fromJson(JsonVariantConst json) override;
  void toJson(JsonObject doc) override;
  String formEntry() const override;

  /** @return Total number of possible enum values. */
  unsigned num_values() const { return m_num_values; }
  /** @return Pointer to the array of value name strings. */
  const char** value_names() const { return m_value_names; }

 protected:
  const unsigned m_num_values;
  const char** m_value_names;
  int m_value;
};

/**
 * @brief Template class for simple enumeration variables.
 */
template <typename T>
class EnumVariable : public EnumVariableBase {
 public:
  EnumVariable(const char* name_, const T& value, const char* description_, unsigned flags_,
               VariableGroup& group)
      : EnumVariableBase(name_, description_, flags_, group), m_value(value) {}
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
  void toJson(JsonObject json) override {
    if (!failed()) {
      json[name()] = static_cast<int>(value());
    }
  }
  bool fromJson(JsonVariantConst json) override {
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

/**
 * @brief Template for enumeration variables using a list of strings for rendering.
 */
template <typename T>
class EnumStrVariable : public EnumStrVariableBase {
 public:
  EnumStrVariable(const char* name_, const T& value, const char* description_, const T& max_value,
                  const char* value_names[], unsigned flags_, VariableGroup& group)
      : EnumStrVariableBase(name_, static_cast<int>(value), description_,
                            static_cast<unsigned>(max_value) + 1, value_names, flags_, group) {}
  const T value() const { return static_cast<T>(m_value); }
  T value() { return static_cast<T>(m_value); }
  EnumStrVariable<T>& operator=(const T value) {
    m_value = static_cast<int>(value);
    setFailed(false);
    return *this;
  }
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
inline bool Variable<bool>::fromJson(JsonVariantConst json) {
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
inline void Variable<String>::toJson(JsonObject json) {
  if (!failed()) {
    json[name()] = value().c_str();
  }
}

template <typename T>
inline void Variable<T>::toJson(JsonObject json) {
  if (!failed()) {
    json[name()] = value();
  }
}

template <typename T>
inline void FloatingPointVariable<T>::toJson(JsonObject json) {
  if (!failed()) {
    json[name()] = value();
  }
}

template <>
inline bool Variable<String>::fromJson(JsonVariantConst json) {
  if (!json.is<const char*>()) {
    return false;
  }
  m_value = json.as<const char*>();
  return true;
}

template <typename T>
inline bool Variable<T>::fromJson(JsonVariantConst json) {
  if (!json.is<T>()) {
    return false;
  }
  m_value = json.as<T>();
  return true;
}

template <typename T>
inline bool FloatingPointVariable<T>::fromJson(JsonVariantConst json) {
  if (!json.is<T>()) {
    setFailed();
    return false;
  }
  m_value = json.as<T>();
  return true;
}

using FloatVariable = FloatingPointVariable<float>;
using DoubleVariable = FloatingPointVariable<double>;

/**
 * @brief Boolean variable type.
 */
class BoolVariable : public Variable<bool> {
 public:
  BoolVariable(const char* name_, const bool value, const char* description_, unsigned flags_,
               VariableGroup& group)
      : Variable<bool>(name_, value, "", description_, flags_, group) {}
  String string() const final { return value() ? "true" : "false"; }
  void toJson(JsonObject json);
  BoolVariable& operator=(bool value);
  String formEntry() const override;
};

/**
 * @brief Binary sensor variable (ON/OFF display).
 */
class BinarySensorVariable : public Variable<bool> {
 public:
  BinarySensorVariable(const char* name_, const bool value, const char* description_,
                       VariableGroup& group, bool publish = true)
      : Variable<bool>(name_, value, "", description_, publish ? 0 : kNoPublish, group) {}
  String string() const override { return value() ? "ON" : "OFF"; }
  void toJson(JsonObject json);
  bool fromJson(JsonVariantConst json);
  BinarySensorVariable& operator=(bool value);
};

/**
 * @brief Binary cover sensor variable (open/closed display).
 */
class BinaryCoverSensorVariable : public BinarySensorVariable {
 public:
  BinaryCoverSensorVariable(const char* name_, const bool value, const char* description_,
                            VariableGroup& group, bool publish = true)
      : BinarySensorVariable(name_, value, description_, group, publish) {}
  String string() const final { return value() ? "open" : "closed"; }
  BinaryCoverSensorVariable& operator=(bool value);
};

}  // namespace og3
