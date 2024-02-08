// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/web_app.h"

namespace og3 {

#ifdef NATIVE
WebApp::WebApp(const WifiApp::Options& options) : WifiApp(options) {}
#else
WebApp::WebApp(const WifiApp::Options& options)
    : WifiApp(options), m_web_server(&module_system()) {}
#endif

}  // namespace og3
