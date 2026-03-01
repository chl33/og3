// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <Arduino.h>

#include <functional>

#include "og3/wifi.h"

#if defined(ESP32)
#include <PsychicHttp.h>
#include <esp_err.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#endif

#ifndef ESP_OK
#define ESP_OK 0
#endif

#ifdef NATIVE
class AsyncWebServerRequest;
class AsyncWebServer;
#define PROGMEM
#endif

namespace og3 {

#if defined(ESP32)
#include <esp_err.h>
using NetRequest = PsychicRequest;
using NetServer = PsychicHttpServer;
using NetHandlerStatus = int;
#else
using NetRequest = AsyncWebServerRequest;
using NetServer = AsyncWebServer;
using NetHandlerStatus = void;
#endif

// Helper to return the correct status from a web handler.
#if defined(ESP32)
#include <esp_err.h>
#define NET_REPLY(VAL) return (VAL)
#else
#define NET_REPLY(VAL) \
  do {                 \
    (void)(VAL);       \
    return;            \
  } while (0)
#endif

using NetHandler = std::function<NetHandlerStatus(NetRequest*)>;

void sendWrappedHTML(NetRequest* request, const char* title, const char* footer,
                     const char* content);
void htmlRestartPage(NetRequest* request, class Tasks* tasks);

extern const char reboot_page[] PROGMEM;
extern const char html_page_template[] PROGMEM;

#define HTML_BUTTON(PATH, TEXT) \
  "<p><form action='" PATH "' method='get'><button>" TEXT "</button></form></p>\n"

class WebButton {
 public:
  WebButton(NetServer* server, const char* label, const char* path, const NetHandler& action);
  void add_button(String* html);

  WebButton& operator=(const WebButton&) = default;

 private:
  const char* m_label;
  const char* m_path;
};

}  // namespace og3
