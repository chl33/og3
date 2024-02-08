// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/app.h"

#include "og3/serial_logger.h"

namespace og3 {

App::App(const Options& options)
    : m_options(options),
      m_logger(options.log_type == LogType::kSerial ? reinterpret_cast<Logger*>(&m_serial_logger)
                                                    : reinterpret_cast<Logger*>(&m_null_logger)),
      m_module_system(m_logger, options.reserve_num_modules),
      m_tasks(options.reserve_tasks, &m_module_system) {}

}  // namespace og3
