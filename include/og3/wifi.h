// Copyright (c) 2026 Chris Lee and contibuters.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

/**
 * @brief Platform-agnostic include for WiFi headers.
 *
 * Automatically includes <WiFi.h> for ESP32 or <ESP8266WiFi.h> for ESP8266.
 */

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#endif
