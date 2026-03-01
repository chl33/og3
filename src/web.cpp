// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#ifndef NATIVE
#include "og3/web.h"

#include "og3/tasks.h"

namespace og3 {

void sendWrappedHTML(NetRequest* request, const char* title, const char* footer,
                     const char* content) {
#if defined(ESP32)
  String html = html_page_template;
  html.replace("%TITLE%", title);
  html.replace("%CONTENT%", content);
  html.replace("%FOOTER%", footer);
  request->reply(200, "text/html", html.c_str());
#else
  auto processor = [title, footer, content](const String& var) -> String {
    if (var == "TITLE") {
      return title;
    } else if (var == "FOOTER") {
      return footer;
    } else if (var == "CONTENT") {
      return content;
    }
    return "";
  };
  request->send_P(200, "text/html", html_page_template, processor);
#endif
}

void htmlRestartPage(NetRequest* request, Tasks* tasks) {
  sendWrappedHTML(request, "reboot", "", reboot_page);
  tasks->runIn(500, [] {
#ifdef ESP32
    ESP.restart();
#else
    ESP.reset();
#endif
  });
}

const char reboot_page[] PROGMEM =
    "<p>Reboot in <span id='count'>10</span></p>"
    "<script>"
    "var count = 10;"
    "(function countDown() {"
    "document.getElementById('count').innerHTML = count + 's';"
    "setTimeout(function() {"
    "if(count > 0) {"
    "count --;"
    "return countDown();"
    "} else {"
    "window.location.href = '/';"
    "}"
    "}, 1000);"
    "})();"
    "</script>";

const char html_page_template[] PROGMEM = R"====(<!DOCTYPE html>
<html lang="en" class="">
<head>
<meta charset='utf-8'>
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<link rel="stylesheet" href="/static/styles.css">
<title>%TITLE%</title>
</head>
<body>
<div class="main">
<div style='text-align:center;'>
  <noscript>To use this page, you need to activate Javascript<br></noscript>
  <h3>%TITLE%</h3>
</div>
%CONTENT%
<br>
<div style='text-align:right;font-size:10px;color: grey;'>
  <hr>%FOOTER%</div>
</div>
</div>
</body>
</html>
)====";

WebButton::WebButton(NetServer* server, const char* label, const char* path,
                     const NetHandler& action)
    : m_label(label), m_path(path) {
  server->on(path, HTTP_GET, action);
}

void WebButton::add_button(String* html) {
  *html += "<p><form action='";
  *html += m_path;
  *html += "' method='get'><button>";
  *html += m_label;
  *html += "</button></form></p>\n";
}

}  // namespace og3

#endif
