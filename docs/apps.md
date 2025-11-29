## App

An [`App`](../include/og3/app.h) object is a container for a set of utility objects which are useful for assembling an application as a set of modules.

The basic `App` contains a:
- [`ModuleSystem`](modules.md) for organizing `Module`s, an
- [`Tasks`](../include/og3/tasks.h) module for running scheduled tasks, and a
- [`Logger`](logging.md) object for logging text messages for monitoring and debugging.

For an application which uses Wifi, a `WifiApp` adds these modules:
- [`WifiManager`](../include/og3/wifi_manager.h) for configuring and establishing connections access points,
- [`Mdns`](../include/og3/mdns.h) which uses mDNS to advertise the device on the local network,
- [`OtaManager`](../include/og3/ota_manager.h) to facilitate updating firmware over the network, and
- [`ConfigInterface`](../include/og3/config_interface.h) for persisting configuration settings via flash storage.

For an application with a web interface, [`WebApp`](../include/og3/web_app.h) adds a `WebServer` which manages an [`AsyncWebServer`](https://github.com/esphome/ESPAsyncWebServer) object.

For an application that supports [MQTT](https://en.wikipedia.org/wiki/MQTT), particularly for talking to [Home Assistant](https://www.home-assistant.io/), a [`HAApp`](../include/og3/ha_app.h) can be used.  This adds a [`MqttManager`](../include/og3/mqtt_manager.h) to help interfacing with a MQTT broker, a [`HADiscovery`](../include/og3/ha_discovery.h) object which assists in declaring Home Assistant Entities from `Variable`s used by the system.  An [`AppStatus`](../include/og3/app_status.h) module automatically publishes basic application stats to the MQTT broker.
