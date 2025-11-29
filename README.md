# og3 library

The **og3** library is a robust C++ application framework for ESP32 and ESP8266 microprocessors. It is designed to simplify the development of MQTT-based IoT applications, particularly those integrating with [Home Assistant](https://www.home-assistant.io/).

This library is built for the [PlatformIO](https://platformio.org/) development environment.

Why "og" ([Garden133 Firmware](https://selectiveappeal.org/posts/garden133-firmware/))?

> "og" stands for [original gangsta](https://www.dictionary.com/e/slang/og/) because it is an old-school C++ library and application framework, not one of those trendy [YAML](https://en.wikipedia.org/wiki/YAML)-based frameworks like [ESPHome](https://esphome.io/index.html) that the kids these days are using. This is partly because I want the control that writing firmware directly in C++ provides, and partly because I'm a programmer so my hobby projects involve real programming, dammit!

*Note: This code is a work in progress and should be considered beta quality. It is suitable for DIY projects, but APIs are still evolving.*

## Key Features

*   **Module System**: Break your application into reusable, independent [`Module`s](docs/modules.md) with managed life-cycles and dependencies.
*   **Home Assistant Integration**: Native support for MQTT discovery, making it easy to expose sensors, switches, and other entities to Home Assistant.
*   **Configuration Management**: Built-in support for saving/loading variables from flash storage and configuring them via a web interface.
*   **Task Scheduling**: Efficient [scheduler](docs/scheduled-tasks.md) for running timed tasks without blocking the main loop.
*   **Web Interface**: Automatic generation of configuration web pages.
*   **Logging**: Simple [logging system](docs/logging.md) supporting Serial and UDP logging.

## Getting Started

To use og3 in your PlatformIO project, add it to your `platformio.ini` `lib_deps`:

```ini
lib_deps =
    chl33/og3
    # Add other dependencies as needed
```

### Minimal Example

Here is a snippet showing how to define a simple module. See the [examples](examples/) folder for complete projects.

```cpp
#include <og3/app.h>
#include <og3/module.h>

class MyModule : public og3::Module {
public:
  MyModule(og3::App* app) : og3::Module("my_module", &app->module_system()) {
    add_init_fn([this]() {
        // Initialization code (e.g., pinMode)
        log()->log("MyModule Initialized");
    });
    add_start_fn([this]() {
        // Startup code
    });
  }
};

og3::App app(og3::App::Options().withLogType(og3::App::LogType::kSerial));
MyModule myModule(&app);

void setup() { app.setup(); }
void loop() { app.loop(); }
```

## Documentation

Detailed documentation for core components:

- [Modules and the ModuleSystem](docs/modules.md)
- [Application Framework (App)](docs/apps.md)
- [Variables](docs/variables.md)
- [Scheduled Tasks](docs/scheduled-tasks.md)
- [Logging](docs/logging.md)

## Examples

Check the `examples/` directory for ready-to-run projects:

- **[Blink](examples/blink/blink.cpp)**: A simple module that blinks an LED.
- **[Wifi-app](examples/wifi-app/wifi-app.cpp)**: Adds configurable WiFi and flash storage.
- **[Web-app](examples/web-app/web-app.cpp)**: Adds a web interface for WiFi configuration.
- **[HA-app](examples/ha-app/ha-app.cpp)**: A complete MQTT application for Home Assistant.

## Projects using og3

- **[Plant133](https://github.com/chl33/Plant133)**: Plant monitoring and watering system.
- **[Dough133](https://github.com/chl33/Dough133)**: Temperature-controlled sourdough proofing box.
- **[Garage133](https://github.com/chl33/Garage133)**: Garage door automation.
- **[Room133](https://github.com/chl33/Room133)**: Room monitoring sensor node.
    - [Boiler](https://github.com/chl33/Boiler): Specialized boiler water level monitor.
- **[Garden133](https://github.com/chl33/Garden133)**: Soil moisture monitoring for gardens.
- **[LoRa133](https://github.com/chl33/LoRa133)**: LoRa base station for Garden133.

## Ecosystem

Special-purpose libraries designed to work with og3:

- **[og3x-shtc3](https://github.com/chl33/og3x-shtc3)**: SHTC3 temperature/humidity sensors.
- **[og3x-bme280](https://github.com/chl33/og3x-bme280)**: BME280 environmental sensors.
- **[og3x-lora](https://github.com/chl33/og3x-lora)**: LoRa radio modules.
- **[og3x-oled](https://github.com/chl33/og3x-oled)**: 0.91" OLED displays.
- **[og3x-satellite](https://github.com/chl33/og3x-satellite)**: Base-station/satellite protocol support.

## Author

**chl33** (https://selectiveappeal.org/)
