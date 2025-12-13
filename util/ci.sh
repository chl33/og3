#! /bin/sh
set -e
cd "$(dirname "$0")"/..

# Build for d1_mini (ESP8266)
# Ignores AsyncTCP (for ESP32) and AsyncTCP_RP2040W (for Pico W)
echo "Building for d1_mini..."
BOARD=d1_mini
FLAGS='lib_ignore=AsyncTCP_RP2040W,AsyncTCP'
COMPAT='lib_compat_mode=strict'
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" --project-option="${COMPAT}" examples/blink/blink.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" --project-option="${COMPAT}" examples/wifi-app/wifi-app.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" --project-option="${COMPAT}" examples/web-app/web-app.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" --project-option="${COMPAT}" examples/ha-app/ha-app.cpp

# Build for esp32dev (ESP32)
# Ignores AsyncTCP_RP2040W (for Pico W)
echo "Building for esp32dev..."
pio run -e esp32dev_blink
pio run -e esp32dev_wifi_app
pio run -e esp32dev_web_app
pio run -e esp32dev_ha_app

pio run -e lolin_c3_mini_blink
pio run -e lolin_c3_mini_wifi_app
pio run -e lolin_c3_mini_web_app
pio run -e lolin_c3_mini_ha_app
