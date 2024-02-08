## Logging

The `og3` library has a very simple logging system.

Typically, a logger is created by the `App` object, based on the value of the `log_type` field of the `App::Options` object passed to its constructor.

If `LogType::kNone` is passed to the `App` constructor, a logger with no output will be used. `LogType::kSerial` generates a logger which outputs tothe serial port.  `LogType::kUdp` generates a logger which writes sends simple UDP messages the the logged text to a secified endpoint.


Example log statements:
```
  log().log("A simple text message").
  log().logf("%u log message with %s", 1, "formatting");
  log().debugf("Only logged if '%s' is defined at compile-time", "LOG_DEBUG");
```

If `LOG_DISABLE` is defined at compile time, no log statements will be part of the code.
