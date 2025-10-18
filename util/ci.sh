#! /bin/sh
set -e
cd "$(dirname "$0")"/..
BOARD=d1_mini
#BOARD=esp32dev
pio ci --board "${BOARD}" --lib . examples/blink/blink.cpp
pio ci --board "${BOARD}" --lib . examples/wifi-app/wifi-app.cpp
pio ci --board "${BOARD}" --lib . examples/web-app/web-app.cpp
pio ci --board "${BOARD}" --lib . examples/ha-app/ha-app.cpp
