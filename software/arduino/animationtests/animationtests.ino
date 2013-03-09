// Get this library from http://bleaklow.com/files/2010/Task.tar.gz (and fix WProgram.h -> Arduino.h)
// and read http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html for background and instructions
#include <Task.h>
#include <TaskScheduler.h>

#define SLEEP_DEBUG_PIN A0
#include "sleep_task.h"
SleepTask sleeper;

#include "animation_tasks.h"
#include "animations.h"
AnimationRunner anim_runner;

void load_nth_animation(uint8_t n)
{
    Serial.print(F("load_nth_animation() called with n="));
    Serial.println(n, DEC);
    load_animation_to_buffer(&FIRST_ANIMATION);
    uint8_t i = 0;
    while (true)
    {
        Serial.print(F("i="));
        Serial.println(i, DEC);

        if (i == n)
        {
            anim_runner.set_animation(&animation_buffer);
            break;
        }

        Serial.print(F("Next (address)=0x"));
        Serial.println((uint16_t)&animation_buffer.Next, HEX);

        if (!animation_buffer.Next)
        {
            Serial.print(F("No Next animation defined, aborting at i="));
            Serial.println(i, DEC);
            break;
        }
        load_animation_to_buffer((const Animation*)animation_buffer.Next);
        i++;
    }
    Serial.println(F("load_nth_animation() exiting"));
}

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
    //load_animation_to_buffer(&foo_anim);
    //anim_runner.set_animation(&foo_anim);
    load_nth_animation(1);
//    load_nth_animation(0);
    // Tasks are in priority order, only one task is run per tick, be sure to keep sleeper as last task if you use it.
    Task *tasks[] = { &anim_runner, &sleeper };
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    // Run the scheduler - never returns.
    Serial.println(F("Scheduler starting"));
    sched.run();
    Serial.println(F("Scheduler exited (this should NOT happen)"));
}
