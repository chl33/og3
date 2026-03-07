# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.5.0] - 2026-03-07

### Added

- **Use PsychicHttp and PsychicMqttClient on ESP32**. These should be more robust on ESP32. On ESP8266, ESPAsyncWebServer and AsyncMqttClient continue to be used.
- **Networking Abstraction Layer**: Standardized `NetHandler` and `NetJsonHandler` signatures to `(NetRequest*, NetResponse*)` for cross-platform compatibility between ESP32 (`PsychicHttp`) and ESP8266 (`ESPAsyncWebServer`).
- **Improved Test Coverage**: Added comprehensive native unit tests for `PID` controllers, `HtmlTable` generation, `Relay` control, and `BlinkLed` patterns.
- **Library Metadata**: Refined `library.json` with descriptive keywords, explicit example declarations, and an improved project summary for better discovery in the PlatformIO Registry.
- **Doxygen Documentation**: Upgraded all header files in `include/og3/` to use Doxygen-style comments, providing detailed documentation for classes, methods, and parameters.
- **GitHub Actions CI**: Automated the CI workflow to run native unit tests and build all examples on every push and pull request using `util/ci.sh`.

### Changed
- **Web Server Configuration**: Increased default `max_uri_handlers` to 20 on ESP32 to prevent 404 errors in complex applications with many routes.
- **Example Updates**: All examples (`ha-app`, `web-app`, `wifi-app`, `blink`) modernized to use the new networking API.
- **PID Defaults**: Updated `PID::Gains` to use more sensible default command limits (`-1e10` to `1e10`) to avoid unintentional output clamping.

### Fixed
- **Anti-Windup Logic**: Verified and refined PID integral accumulation logic to correctly pause when the command output is saturated.
- **ESP8266 Compatibility**: Fixed `AsyncCallbackJsonWebHandler` namespace issues and updated wrapper lambdas to support the new unified handler signature.
- **PID Gain Limits**: Fixed a bug where `command_min` and `command_max` defaults were inverted in the `Gains` structure.
