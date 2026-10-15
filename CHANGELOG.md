# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.7.0] - 2026-10-14

### Added
- **NTP Time Synchronization (`Ntp`)**: Added the `Ntp` module to synchronize system time via SNTP after WiFi connection, providing `isSynced()`, configurable time servers, and timezone string options (`og3/ntp.h`).
- **Offline UDP Log Buffering**: `UdpLogger` now uses a fixed-capacity ring queue to buffer log entries with timestamps while WiFi is offline, automatically flushing queued logs in batched datagrams upon reconnection.
- **Adafruit IO & SSL/TLS MQTT Support**: Enhanced `MqttManager` with support for Adafruit IO (`Mode::kAdafruitIO`) alongside Home Assistant, configurable SSL/TLS options, and Adafruit root CA bundle verification on ESP32.
- **Pointer-Based Dependency Linking**: Added `Module::add_dependency(Module*)` to declare dependencies directly on module pointers.

### Changed
- **WifiManager State Machine & Exponential Backoff**: Restructured `WifiManager` into a formal state machine (`kIdle`, `kConnectingSta`, `kConnectedSta`, `kWaitingReconnectSta`, `kSoftAp`) with exponential backoff retries, automatic Soft AP failover, and periodic reconnect attempts.
- **Module & Variable Identity**: Updated `Module` constructors and `VariableBase` metadata to use `std::string` internally to ensure safe lifecycle and memory management.
- **Deleted Copy Constructors**: Explicitly deleted copy constructors and assignment operators for `Module`, `VariableBase`, `VariableGroup`, `App`, and related classes to prevent accidental slicing or lifecycle duplication.
- **Sonar Pulse Timeout**: Added a configurable pulse timeout to `Sonar::ping()` to prevent blocking if an echo pulse is missed.
- **Scoped MQTT Status Publishing**: Restricted periodic `AppStatus` and `WifiMonitor` MQTT status reporting to Home Assistant mode, avoiding feed spamming in Adafruit IO mode.

## [0.6.4] - 2026-04-04

### Changed
- **PIR, DIN**: allow setting pin mode on init
- **WebApp**: always reboot board when wifi is configured

## [0.6.3] - 2026-04-02

### Changed
- **MappedAanalogSensor**: export variables as camelCase
- **Adc**: read value on init
- **HADiscovery**: accessor for MqttConnected()

## [0.6.2] - 2026-03-30

### Changed
- **PID, PWM**: PID exports variables as camelCase, added some accessors.

## [0.6.1] - 2026-03-24

### Added
- **Extended ConfigInterface**: Added `read_file(filename, &content)` and `write_file(filename, content)` methods to support arbitrary JSON and text data storage on the flash filesystem.
- **Improved Native Support**: Enhanced the `LittleFS` mock in `config_interface.cpp` to provide better file IO behavior for unit tests and local development.
- **HA Discovery**: Allow overriding the device-availability MQTT topic.

## [0.6.0] - 2026-03-24

### Added
- **Declarative Dependency Injection**: Introduced the `require<T>(name, &ptr)` pattern. Modules can now declare their dependencies and receive pointers to them directly in the constructor, with resolution safely deferred until after all modules are constructed.
- **Implicit Dependency Graph**: The `ModuleSystem` now automatically builds the topological sort graph from `require()` calls, significantly reducing the need for manual `setDependencies()` calls.
- **Memory Efficiency (ESP8266)**: Dependency metadata is now stored in transient contiguous memory during boot and fully reclaimed before the main loop starts, preventing heap fragmentation on memory-constrained devices.
- **CamelCase JSON Standard**: Transitioned core framework JSON keys and public method names to `camelCase` (e.g., `ipAddr`, `wifiPassword`, `mqttConnected`) to provide a native experience for Svelte and JavaScript frontends.

### Changed
- **Unified Header Structure**: Merged `variable_group.h` into `variable.h` to simplify include management.
- **Modernized Core Modules**: `WifiManager`, `MqttManager`, and `AppStatus` refactored to use the new `require()` pattern and `camelCase` API.
- **Refined Module Lifecycle**: Simplified the internal `link()` phase by replacing legacy manual linking lambdas with automated declarative resolution.

### Removed
- ** Purged Legacy API**: Removed `Module::add_link_fn()`, `Module::setDependencies()`, and the `HADependencies` helper class.
- **Standalone Dependency Headers**: Removed `og3/include/og3/dependencies.h` and associated source files as they are no longer required for the modernized dependency system.

## [0.5.0] - 2026-03-07

### Added

- **Use PsychicHttp and PsychicMqttClient on ESP32**. These should be more robust on ESP32. On ESP8266, ESPAsyncWebServer and AsyncMqttClient continue to be used.
- **Networking Abstraction Layer**: Standardized `NetHandler` and `NetJsonHandler` signatures to `(NetRequest*, NetResponse*)` for cross-platform compatibility between ESP32 (`PsychicHttp`) and ESP8266 (`ESPAsyncWebServer`).
- **Improved Test Coverage**: Added comprehensive native unit tests for `PID` controllers, `HtmlTable` generation, `Relay` control, and `BlinkLed` patterns.
- **Library Metadata**: Refined `library.json` with descriptive keywords, explicit example declarations, and an improved project summary for better discovery in the PlatformIO Registry.
- **Doxygen Documentation**: Upgraded all header files in `include/og3/` to use Doxygen-style comments, providing detailed documentation for classes, methods, and parameters.
- **GitHub Actions CI**: Automated the CI workflow to run native unit tests and build all examples on every push and pull request using `util/ci.sh`.
- **Static Code Analysis**: Integrated `cppcheck` into the PlatformIO workflow with targeted filters to ensure code quality and identify potential bugs early.

### Changed
- **Web Server Configuration**: Increased default `max_uri_handlers` to 20 on ESP32 to prevent 404 errors in complex applications with many routes.
- **Example Updates**: All examples (`ha-app`, `web-app`, `wifi-app`, `blink`) modernized to use the new networking API.
- **PID Defaults**: Updated `PID::Gains` to use more sensible default command limits (`-1e10` to `1e10`) to avoid unintentional output clamping.

### Fixed
- **Anti-Windup Logic**: Verified and refined PID integral accumulation logic to correctly pause when the command output is saturated.
- **ESP8266 Compatibility**: Fixed `AsyncCallbackJsonWebHandler` namespace issues and updated wrapper lambdas to support the new unified handler signature.
- **PID Gain Limits**: Fixed a bug where `command_min` and `command_max` defaults were inverted in the `Gains` structure.
