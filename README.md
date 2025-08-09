# og3 library

The og3 library is a C++ application framework for ESP microprocessors.  It was written especially to support MQTT applications and interface with [Home Assistant](https://www.home-assistant.io/).  This library is designed to be used with the [Platformio](https://platformio.org/) development environment.

Note that this code is a work in progress and should be considered beta quality.  It is suitable for DIY projects at this point.  The APIs of this code are still slowly evolving over time.

- [Modules and the ModuleSystem](docs/modules.md).  This system helps to break an application into a set of reusable modules which can work together.
- [Tasks](docs/scheduled-tasks.md).  This is a utility for running scheduled tasks.
- [Logging](docs/logging.md).  This is a very simple text-logging system for monitoring and debugging.
- [Variables](docs/variables.md). A `Variable` is a named container for a value, used with web forms, MQTT, and flash storage.
- [Application framework](docs/apps.md).  An `App` is a package of modules, a system for running scheduled jobs, logging, and other utilities to help writing a firmware application.

Example programs:
- [Blink](examples/blink/blink.cpp) is a simple application with a module that can blink an LED on and off.
- [Wifi-app](examples/wifi-app/wifi-app.cpp) is like Blink, but adds a configurable Wifi connection and flash storage.
- [Web-app](examples/web-app/web-app.cpp) adds to Wifi-app a web interface for configuring the Wifi options.
- [HA-app](examples/ha-app/ha-app.cpp) is a simple application using MQTT, for talking to [Home Assistant](https://www.home-assistant.io/).  It builds on Web-app.

See a description of using this application framework in [Garden133 Firmware](https://selectiveappeal.org/posts/garden133-firmware/).

## Projects which use og3

- [Plant133](https://github.com/chl33/Plant133) is a device that can monitor and water up to 4 plants at a time.
- [Dough133](https://github.com/chl33/Dough133) is a temperature-controlled container for proofing and fermenting soughdough.
- [Garage133](https://github.com/chl33/Garage133) is a DIY device for garage door automation.
- [Room133](https://github.com/chl33/Room133) is a device for room monitoring, for use with Home Assistant
    - [Boiler](https://github.com/chl33/Boiler) is software for the Room133 board, which monitors the water level of a boiler.
- [Garden133](https://github.com/chl33/Garden133) is a device that monitors soil moisture in your yard and garden, to help with watering automation.
- [LoRa133](https://github.com/chl33/LoRa133) is a LoRa base-station for Garden133.

chl33
