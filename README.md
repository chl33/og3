# og3 library

The **og3** library is a robust C++ application framework for ESP32 and ESP8266 microprocessors. It is designed to simplify the development of MQTT-based IoT applications, particularly those integrating with [Home Assistant](https://www.home-assistant.io/).

Applications can be organized into separate modules, and modules can use one another. Modules provided include support for WiFi configuration, connections to MQTT brokers, a web interface, OTA updates, simple logging, and support for Home Assistant device discovery using MQTT.

This library is built for the [PlatformIO](https://platformio.org/) development environment.

## What's New in v0.6.0

*   **Declarative Dependencies**: New `require<T>(name, &ptr)` API for automatic, type-safe module linking.
*   **Boilerplate Reduction**: Removal of legacy linking lambdas and manual dependency declarations.
*   **Svelte Alignment**: Core JSON keys and APIs now use `camelCase` for seamless frontend integration.
*   **Memory Efficiency**: Optimized boot-time dependency resolution to minimize heap fragmentation on ESP8266.

## What's New in v0.5.0

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
    // Declaratively require other modules
    require(og3::WifiManager::kName, &m_wifi);

    add_init_fn([this]() {
        // Initialization code (e.g., pinMode)
        if (m_wifi) {
          log()->log("MyModule Initialized with WiFi");
        }
    });
  }
private:
  og3::WifiManager* m_wifi = nullptr;
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
