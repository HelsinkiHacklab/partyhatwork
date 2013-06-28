#ifndef SLEEP_TASKS_H
#define SLEEP_TASKS_H
#include <Arduino.h>
#include <Task.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

class SleepTask : public Task
{
public:
    SleepTask();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
};

SleepTask::SleepTask()
: Task()
{
}

bool SleepTask::canRun(uint32_t now)
{
    return true;
}

void SleepTask::run(uint32_t now)
{
#ifdef SLEEP_DEBUG_PIN
    digitalWrite(SLEEP_DEBUG_PIN, LOW);
#endif
    // Idle sleep, it saves relatively little power but any other mode will disable way too many clocks
    // (at some point if we want to be really smart we can start checking our state and power down more if there is nothing to do)
    set_sleep_mode(SLEEP_MODE_IDLE);
    // Make sure sleeping is enabled and make sure interrupts are enabled (or we'll never wake up)
    sleep_enable();
    sei(); // TODO: doublecheck this, it is from an example but the QP framework uses an assembler macro to make sure the enabling interrupts and going to sleep is atomic (so nothing can disable them in between)
    sleep_cpu();
    // Disable sleep when coming out, this is recommended by the documentation
    sleep_disable();
#ifdef SLEEP_DEBUG_PIN
    digitalWrite(SLEEP_DEBUG_PIN, HIGH);
#endif
}

SleepTask sleeper;
extern SleepTask sleeper;


#endif
