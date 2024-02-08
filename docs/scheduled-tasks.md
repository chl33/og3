## Scheduled tasks

Embedded applications often involve actions according to a schedule, such as an LED blinking on and off at a given rate, or sensor readings taken at a given frequency.  The [`Tasks`](../include/og3/tasks.h) module is a simple utility for managing these kinds of tasks.

Code can register callbacks to be called via `Tasks` using the methods `tasks.runAt()` and `tasks.runIn()`.  These callbacks will be invoked at the specified times.  The module contains an [`TaskQueue`](../include/og3/task_queue.h) object, which maintains a priority queue of timed callbacks to be run.  The `TaskQueue` object has a fixed number of tasks which it can track at a given time.  This number is specified when the `TaskQueue` is created.

For example, in the [Blink](../examples/blink/blink.cpp) example, the `Tasks` object that is part of an `App` is used to schedule the method `Bink::blink()` to be called every second to toggle the state of an LED.

```C++
namespace {

og3::App s_app(og3::App::Options().withLogType(og3::App::LogType::kSerial));

class Blink : public og3::Module {
 public:
  explicit Blink(og3::App* app) : og3::Module("blink", &app->module_system()), m_app(app) {
    // During app initialization, set the LED pin to output mode.
    add_init_fn([this]() { pinMode(kLEDPin, OUTPUT); });
    // When the app starts, start blinking.
    add_start_fn([this]() { blink(); });
  }
  // The blink function is first called, on start(), and afterward it is called via a
  //  timed task.
  // After toggling the LED output pin, we schedule the function to be called again in
  //  one second (1000 msec), then we log the state of the output pin to the serial port.
  void blink() {
    digitalWrite(kLEDPin, m_high ? HIGH : LOW);
    m_high = !m_high;
    m_app->tasks().runIn(1000, [this]() { blink(); });
    m_app->log().logf("blink: %s", m_high ? "on" : "off");
  }

 private:
  og3::App* m_app;
  bool m_high = false;
};

Blink s_blink(&s_app);

}  // namespace

void setup() { s_app.setup(); }
void loop() { s_app.loop(); }
```

Sometimes you want to make sure that you don't queue more than one kind of callback at a time.  For example, if some other code invoked `blink()` in the example above after the program started, then there would be two `blink()` callbacks scheduled to run at a time, and the LED would blink roughly twice as fast.  If `blink()` mistakenly called `m_app->tasks().runIn(1000, [this]() { blink(); });` multiple times when it was invoked, the number of callbacks would double every second until the task queue reached the total number of tasks it supported at a given time.

A way to solve this is to register a timed callback with an `id` value.  In this case, only one callback with a given `id` may be in the queue at a time.  If callback is queued to run with a given `id' value, then the callback replaces any preexisting timed callback with the same ID.

A simple way to use this is an `TaskScheduler`, which automatically obtains a unique ID from the `Tasks` module and schedules tasks with the `Tasks` module using that `id`.

```C++
	TaskScheduler sched(&s_app.tasks()):
	sched.runIn9(50, my_callback);
```

For a periodic task, `PeriodicTaskScheduler` can be used.  The following expression will schedule `sendStatus()` to run 1 second after the code starts and then every 10 seconds thereafter.
```C++
PeriodicTaskScheduler peq(kMsecInSec, 10 * kMsecInSec, sendStatus, &s_app.tasks());
```
