#ifndef ANIMATION_TASKS_H
#define ANIMATION_TASKS_H
#include <Arduino.h>
#include <Task.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

typedef struct {
    void *Next; // Pointer to next animation
    uint8_t leds; // bits for led 0-7
    uint8_t length; // Frames
    uint8_t modes; // Bit flags, will be later to used to indicate fade mode etc
    uint16_t first_frame;
    
} Animation_PGM PROGMEM;


class AnimationRunner : public TimedTask
{
public:
    // Create a new blinker for the specified pin and rate.
    AnimationRunner();
    virtual void run(uint32_t now);
    uint32_t on_time; // Blink rate.
    uint8_t pin;      // LED pin.
    uint32_t binterval;    // Interval 
private:
    bool on;          // Current state of the LED.
};

AnimationRunner::AnimationRunner()
: TimedTask(millis()),
  on(false)
{
}

void AnimationRunner::run(uint32_t now)
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
