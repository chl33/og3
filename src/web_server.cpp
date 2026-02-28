// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/web_server.h"

#include "og3/config_interface.h"
#include "og3/wifi_manager.h"

namespace og3 {

const char* WebServer::kName = "web_server";

bool read(NetRequest& request, VariableBase& var) {
#ifndef NATIVE
#if defined(ESP32)
  if (!request.hasParam(var.name())) {
    return false;
  }
  return var.fromString(request.getParam(var.name())->value());
#else
  if (!request.hasParam(var.name(), true)) {
    return false;
  }
  return var.fromString(request.getParam(var.name(), true)->value());
#endif
#else
  return true;
#endif
}
bool read(NetRequest& request, const VariableGroup& var_group) {
#ifndef NATIVE
  bool ret = true;
  for (auto* var : var_group.variables()) {
    if (var->settable()) {
      ret = read(request, *var) && ret;
    }
  }
  return ret;
#else
  return true;
#endif
}

WebServer::WebServer(ModuleSystem* module_system, uint16_t port)
    : Module(WebServer::kName, module_system)
#ifndef NATIVE
#if !defined(ESP32)
      ,
      m_server(port)
#endif
#endif
{
  add_link_fn([this](NameToModule& name_to_module) -> bool {
    m_wifi_manager = WifiManager::get(name_to_module);
    if (!m_wifi_manager) {
      return false;
    }
#ifndef NATIVE
    m_wifi_manager->addConnectCallback([this]() {
#if defined(ESP32)
      m_server.listen(80);
#else
      m_server.begin();
#endif
      log()->log("Web server started.");
    });
#endif
    return true;
  });
}

#ifndef NATIVE
void WebServer::on(const char* uri, NetHandler handler) {
#if defined(ESP32)
  m_server.on(uri, HTTP_GET, handler);
#else
  m_server.on(uri, HTTP_GET, handler);
#endif
}

void WebServer::on(const char* uri, NetHandler handler, NetUploadCallback upload_handler) {
#if defined(ESP32)
  PsychicUploadHandler* uh = new PsychicUploadHandler();
  uh->onUpload(upload_handler);
  m_server.on(uri, HTTP_POST, uh);
#else
  m_server.on(uri, HTTP_POST, handler, upload_handler);
#endif
}

void WebServer::onNotFound(NetHandler handler) {
#if defined(ESP32)
  m_server.onNotFound(handler);
#else
  m_server.onNotFound(handler);
#endif
}
#endif

}  // namespace og3
