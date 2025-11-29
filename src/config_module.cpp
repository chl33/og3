// Copyright (c) 2025 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/config_module.h"

#include "og3/html_table.h"

namespace og3 {

ConfigModule::ConfigModule(const char* name, App* app)
    : Module(name, &app->module_system()),
      m_app(app),
      m_cvg(name),
      m_cfg_url(std::string("/config/") + name) {
  setDependencies({ConfigInterface::kName, WebServer::kName});
  add_link_fn([this](og3::NameToModule& name_to_module) -> bool {
    m_config = ConfigInterface::get(name_to_module);
    m_web_server = WebServer::get(name_to_module);
    return m_config && m_web_server;
  });
  add_init_fn([this]() {
    if (m_config) {
      m_config->read_config(m_cvg);
    }
#ifndef NATIVE
    if (m_web_server) {
      m_web_server->server().on(cfg_url(), [this](AsyncWebServerRequest* request) {
        this->handleConfigRequest(request);
      });
    }
#endif
  });
}

void ConfigModule::add_html_button(String* body) const {
  Module::add_html_button(body, name(), cfg_url());
}

void ConfigModule::handleConfigRequest(AsyncWebServerRequest* request) {
#ifndef NATIVE
  ::og3::read(*request, m_cvg);
  m_html.clear();
  html::writeFormTableInto(&m_html, m_cvg);
  Module::add_html_button(&m_html, "Back", "/");
  sendWrappedHTML(request, m_app->board_cname(), name(), m_html.c_str());
  if (m_config) {
    m_config->write_config(m_cvg);
  }
#endif
}

}  // namespace og3
