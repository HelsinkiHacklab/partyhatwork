#ifndef BLINKER_H
#define BLINKER_H
#include <Task.h>

// Timed task to blink a LED.
class Blinker : public TimedTask
{
public:
    // Create a new blinker for the specified pin and rate.
    Blinker(uint8_t _pin, uint32_t _rate);
    virtual void run(uint32_t now);
    uint32_t on_time; // Blink rate.
private:
    uint8_t pin;      // LED pin.
    uint32_t binterval;    // Interval 
    bool on;          // Current state of the LED.
};

Blinker::Blinker(uint8_t _pin, uint32_t _binterval)
: TimedTask(millis()),
  pin(_pin),
  binterval(_binterval),
  on(false)
{
    // default to 50%
    on_time = binterval;
    pinMode(pin, OUTPUT);     // Set pin for output.
}

void Blinker::run(uint32_t now)
{
    // If the LED is on, turn it off and remember the state.
    if (on) {
        digitalWrite(pin, LOW);
        on = false;
        // Call again after binterval ms
        incRunTime(binterval);
    // If the LED is off, turn it on and remember the state.
    } else {
        digitalWrite(pin, HIGH);
        on = true;
        // call again after on_time ms to turn off
        incRunTime(on_time);
    }
}

#endif
