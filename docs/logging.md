## Logging

The `og3` library has a very simple logging system.

Typically, a logger is created by the `App` object, based on the value of the `log_type` field of the `App::Options` object passed to its constructor.

If `LogType::kNone` is passed to the `App` constructor, a logger with no output will be used. `LogType::kSerial` generates a logger which outputs to the serial port. `LogType::kUdp` generates a logger which sends UDP messages to a specified endpoint. While WiFi is disconnected, UDP log messages are queued in an internal ring buffer with timestamps and automatically flushed when the connection is established.


Example log statements:
```cpp
  log().log("A simple text message");
  log().logf("%u log message with %s", 1, "formatting");
  log().debugf("Only logged if '%s' is defined at compile-time", "LOG_DEBUG");
```

If `LOG_DISABLE` is defined at compile time, no log statements will be part of the code.
