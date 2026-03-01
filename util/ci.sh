#! /bin/sh
set -e
cd "$(dirname "$0")"/..

# Build for d1_mini (ESP8266)
# Ignores AsyncTCP (for ESP32) and AsyncTCP_RP2040W (for Pico W)
echo "Building for d1_mini..."
BOARD=d1_mini
FLAGS='lib_ignore=AsyncTCP_RP2040W,AsyncTCP'
COMPAT='lib_compat_mode=strict'

for board in d1_mini esp32dev lolin; do
    for app in blink wifi_app web_app ha_app; do
	pio run -e "${board}_${app}"
    done
done
