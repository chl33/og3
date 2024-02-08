# og3 library

The og3 library is designed to help write applications for ESP microprocessors using the Arduino environment.  It was written especially to support MQTT applications and interface with [Home Assistant](https://www.home-assistant.io/).  This library is designed to be used with the [Platformio](https://platformio.org/) development environment.

Note that this code is a work in progress and should be considered alpha or beta quality.  It is suitable for DIY projects at this point.

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

chl33
