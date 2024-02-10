// Copyright (c) 2024 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <functional>

#include "og3/wifi.h"

#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif
#ifndef NATIVE
#include <ESPAsyncWebServer.h>
#else
class AsyncWebServerRequest;
class AsyncWebServer;
#define PROGMEM
#endif

namespace og3 {

void sendWrappedHTML(AsyncWebServerRequest* request, const char* title, const char* footer,
                     const char* content);
void htmlRestartPage(AsyncWebServerRequest* request, class Tasks* tasks);

extern const char reboot_page[] PROGMEM;
extern const char html_page_template[] PROGMEM;

#define HTML_BUTTON(PATH, TEXT) \
  "<p><form action='" PATH "' method='get'><button>" TEXT "</button></form></p>\n"

class WebButton {
 public:
  using Action = std::function<void(AsyncWebServerRequest*)>;

  WebButton(AsyncWebServer* server, const char* label, const char* path, const Action& action);
  void add_button(String* html);

  WebButton& operator=(const WebButton&) = default;

 private:
  const char* m_label;
  const char* m_path;
};

}  // namespace og3
