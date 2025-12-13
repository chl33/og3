#! /bin/sh
set -e
cd "$(dirname "$0")"/..

# Build for d1_mini (ESP8266)
# Ignores AsyncTCP (for ESP32) and AsyncTCP_RP2040W (for Pico W)
echo "Building for d1_mini..."
BOARD=d1_mini
FLAGS='lib_ignore=AsyncTCP_RP2040W,AsyncTCP'
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" examples/blink/blink.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" examples/wifi-app/wifi-app.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" examples/web-app/web-app.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" examples/ha-app/ha-app.cpp

# Build for esp32dev (ESP32)
# Ignores AsyncTCP_RP2040W (for Pico W)
echo "Building for esp32dev..."
BOARD=esp32dev
FLAGS='lib_ignore=AsyncTCP_RP2040W'
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" examples/blink/blink.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" examples/wifi-app/wifi-app.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" examples/web-app/web-app.cpp
pio ci --board "${BOARD}" --lib . --project-option="${FLAGS}" examples/ha-app/ha-app.cpp
