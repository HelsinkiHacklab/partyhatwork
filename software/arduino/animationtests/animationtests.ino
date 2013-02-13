// Get this library from http://bleaklow.com/files/2010/Task.tar.gz (and fix WProgram.h -> Arduino.h)
// and read http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html for background and instructions
#include <Task.h>
#include <TaskScheduler.h>

#define SLEEP_DEBUG_PIN A0


// Animations
#include "animation_tasks.h"

#include "sleep_task.h"
SleepTask sleeper;


void setup()
{
    Serial.begin(115200);
#ifdef SLEEP_DEBUG_PIN
    pinMode(SLEEP_DEBUG_PIN, OUTPUT);
    // We keep the pin high whenever the sketch is running
    digitalWrite(SLEEP_DEBUG_PIN, HIGH);
#endif
    Serial.println(F("Booted"));
}


void loop()
{
    // Tasks are in priority order, only one task is run per tick, be sure to keep sleeper as last task if you use it.
    Task *tasks[] = { &sleeper };
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    // Run the scheduler - never returns.
    Serial.println(F("Scheduler starting"));
    sched.run();
    Serial.println(F("Scheduler exited (this should NOT happen)"));
}
