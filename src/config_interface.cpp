// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/config_interface.h"

#include <ArduinoJson.h>  // json to process MQTT: ArduinoJson 6.11.4
#ifndef NATIVE
#include <LittleFS.h>
#endif

#include "og3/flash_support.h"
#include "og3/logger.h"
#include "og3/module_system.h"
#include "og3/variable.h"

#ifdef NATIVE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>

#include "errno.h"
#include "string.h"
namespace {
class File {
 public:
  File(const char* filename, const char* mode)
      : m_stream(filename, (mode && mode[0] == 'w') ? std::ios::out : std::ios::in) {}

  operator bool() { return m_stream.is_open(); }
  int read() {
    char r;
    m_stream.get(r);
    return r;
  }
  size_t write(uint8_t c) {
    m_stream.put(c);
    return 1;
  }
  size_t write(const uint8_t* out, size_t len) {
    m_stream.write(reinterpret_cast<const char*>(out), len);
    return len;
  }
  void close() { m_stream.close(); }

 private:
  std::fstream m_stream;
};

class LittleFileFS {
 public:
  LittleFileFS() {}
  File open(const char* filename, const char* mode) { return File(filename, mode); }
  bool exists(const char* filename) {
    struct stat statbuf;
    if (0 != stat(filename, &statbuf)) {
      if (errno == ENOENT) {
        return false;
      }
      printf("Failed to stat '%s': %s\n", filename, strerror(errno));
      return false;
    }
    return true;
  }
};

LittleFileFS LittleFS;

const char kFSRoot[] = "./";
}  // namespace
#else
namespace {
const char kFSRoot[] = "/";
}
#endif

namespace og3 {

const char* ConfigInterface::kName = "config";

ConfigInterface::ConfigInterface(ModuleSystem* module_system)
    : Module(ConfigInterface::kName, module_system), m_dependency(FlashSupport::kName) {
  setDependencies(&m_dependency);
  add_link_fn([this](NameToModule& name_to_module) -> bool {
    m_fs = FlashSupport::get(name_to_module);
    return m_fs != nullptr;
  });
}

Logger* ConfigInterface::log() { return module_system()->log(); }

bool ConfigInterface::read_config(VariableGroup& var_group, const char* filename) {
  // Make sure flash is setup.
  if (!m_fs || !m_fs->setup()) {
    return false;
  }

  char fname[80];
  if (filename) {
    snprintf(fname, sizeof(fname), "%s%s", kFSRoot, filename);
  } else {
    snprintf(fname, sizeof(fname), "%s%s.json", kFSRoot, var_group.name());
  }

  if (!LittleFS.exists(fname)) {
    log()->debugf("Config file '%s' does not exist.", fname);
    return false;
  }
  File config_file = LittleFS.open(fname, "r");
  if (!config_file) {
    log()->debugf("Failed to open config file '%s'.", fname);
    return false;
  }
  log()->debugf("Reading config file '%s'.", fname);
  JsonDocument doc;
  deserializeJson(doc, config_file);
  for (auto* var : var_group.variables()) {
    if (!var->config()) {
      continue;
    }
    const JsonVariant val = (doc)[var->name()];
    if (val.isNull()) {
      continue;
    }
    if (!var->fromJson(val)) {
      log()->logf("Failed to read variable '%s' from '%s'.", var->name(), fname);
    }
  }
  config_file.close();
  return true;
}

bool ConfigInterface::write_config(const VariableGroup& var_group, const char* filename) {
  // Make sure flash is setup.
  if (!m_fs || !m_fs->setup()) {
    return false;
  }

  char fname[80];
  if (filename) {
    snprintf(fname, sizeof(fname), "%s%s", kFSRoot, filename);
  } else {
    snprintf(fname, sizeof(fname), "%s%s.json", kFSRoot, var_group.name());
  }

#if USE_SPIFFS
  File config_file = SPIFFS.open(fname, "w");
#else
  File config_file = LittleFS.open(fname, "w");
#endif
  if (!config_file) {
    log()->logf("Failed to open config file '%s' for writing.", fname);
    return false;
  }
  // Allocate a buffer to store contents of the file.
  JsonDocument jsondoc;
  JsonObject json = jsondoc.to<JsonObject>();
  for (auto* var : var_group.variables()) {
    if (!var->config()) {
      continue;
    }
    var->toJson(json);
  }
  serializeJson(jsondoc, config_file);
  config_file.close();
  return true;
}

}  // namespace og3
