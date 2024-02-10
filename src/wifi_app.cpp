// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/wifi_app.h"

#include <memory>

#include "og3/udp_logger.h"

namespace og3 {

WifiApp::WifiApp(const WifiApp::Options& options)
    : App(options.app),
      m_software_name(options.software_name),
      m_config(&module_system()),
      m_flash_support(&module_system()),
      m_wifi_manager(options.default_device_name, &tasks(), options.wifi),
      m_ota_manager(options.ota, &module_system()),
      m_dns(&tasks()) {
  if (options.app.log_type == LogType::kUdp) {
    m_ptr_logger = std::unique_ptr<UdpLogger>(new UdpLogger(&m_wifi_manager, options.udp_log_host));
    m_logger = m_ptr_logger.get();
  }
}

}  // namespace og3
