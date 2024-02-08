// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "og3/module.h"

#include "og3/module_system.h"

namespace og3 {

Module::Module(const char* name_, ModuleSystem* module_system)
    : m_name(name_), m_module_system(module_system) {
  m_module_system->add_module(this);
}

Logger* Module::log() const { return m_module_system->log(); }

void Module::add_link_fn(const LinkFn& link_fn) { m_module_system->add_link_fn(link_fn, this); }
void Module::add_init_fn(const Thunk& thunk) { m_module_system->add_init_fn(thunk, this); }
void Module::add_start_fn(const Thunk& thunk) { m_module_system->add_start_fn(thunk, this); }
void Module::add_update_fn(const Thunk& thunk) { m_module_system->add_update_fn(thunk, this); }

void Module::add_html_button(String* body, const char* title, const char* url) {
  *body += F("<p><form action='");
  *body += url;
  *body += F("' method='get'><button>");
  *body += title;
  *body += F("</button></form></p>\n");
}

}  // namespace og3
