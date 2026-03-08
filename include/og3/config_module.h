// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.
#pragma once

#include "og3/app.h"
#include "og3/config_interface.h"
#include "og3/dependencies.h"
#include "og3/module.h"
#include "og3/variable.h"
#include "og3/web_server.h"
#include "og3/wifi_app.h"

namespace og3 {

/**
 * @brief ConfigModule is a parent class for modules which save and load their variable values from
 * flash via the config system and should have their own web page for setting variable values.
 */
class ConfigModule : public Module {
 public:
  /**
   * @brief Construct a new ConfigModule object.
   * @param name The name of the module.
   * @param app The application instance.
   */
  ConfigModule(const char* name, App* app);

  /**
   * @brief Get the URL for the configuration page.
   * @return The configuration URL.
   */
  const char* cfg_url() const { return m_cfg_url.c_str(); }

  /**
   * @brief Add an HTML button that links to this module's config page.
   * @param body The HTML body string to append to.
   */
  void add_html_button(String* body) const;

 protected:
  const App* m_app;                     ///< Application instance.
  VariableGroup m_cvg;                  ///< Configuration variable group.
  ConfigInterface* m_config = nullptr;  ///< Config interface instance.
  WebServer* m_web_server = nullptr;    ///< Web server instance.
  std::string m_cfg_url;                ///< URL for this module's config page.
  String m_html;                        ///< HTML content for the config page.

 private:
  /**
   * @brief HTTP handler for configuration requests.
   * @param request The network request object.
   * @param response The network response object.
   * @return The handling status.
   */
  NetHandlerStatus handleConfigRequest(NetRequest* request, NetResponse* response);
};

}  // namespace og3
