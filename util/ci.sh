#! /bin/sh
set -e
cd "$(dirname "$0")"/..

for board in d1_mini esp32dev; do
    echo "-- Building for ${board}..."
    for app in blink wifi_app web_app ha_app; do
	echo " -- Building ${board}_${app}..."
	pio run -e "${board}_${app}"
    done
done
