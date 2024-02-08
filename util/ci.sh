#! /bin/sh
set -e
cd "$(dirname "$0")"/..
pio ci --board d1_mini --lib . examples/blink/blink.cpp
pio ci --board d1_mini --lib . examples/wifi-app/wifi-app.cpp
pio ci --board d1_mini --lib . examples/web-app/web-app.cpp
pio ci --board d1_mini --lib . examples/ha-app/ha-app.cpp
