/**
 * Partyhatwork firmware reference implementation, you will need XMega compatible Arduino IDE, see https://github.com/HelsinkiHacklab/XMega-boards/tree/master/software/arduino-xmega
 * 
 * Ge the XBee library from http://code.google.com/p/xbee-arduino/
 * Get the Task library from http://bleaklow.com/files/2010/Task.tar.gz (and fix WProgram.h -> Arduino.h)
 * 
 
   Arduino Pin map

            USB
    0   PE0     GND
    1   PE1     GND
    2   PE2     PD5   29
    3   PE3     PD4   28
    4   PA0     PD3   27
    5   PA1     PD2   26
    6   PA2     PD1   25
    7   PA3     PD0   24
    8   PA4     PC7   23
    9   PA5     PC6   22
   10   PA6     PC5   21
   11   PA7     PC4   20
   12   PB0     PC3   19
   13   PB1     PC2   18
   14   PB2     PC1   17
   15   PB3     PC0   16

From serial_init.cpp:
SERIAL_DEFINE(Serial2, D, 0); -> PD2/PD3 == 26/27
SERIAL_DEFINE(Serial3, E, 0); -> PE2/PE3 == 2/3

 * 
 */

// If you have EEG module connected, connect it here.
#define BRAIN_SERIAL Serial1

// Get this library from http://code.google.com/p/xbee-arduino/
#include <XBee.h>

#define BAT_SENSE_PIN A9
#define HIGH_CURRENT_CHG_PIN 12
#define XBEE_RESET_PIN 28
// This will be HIGH whenever we are *not* sleeping
#define SLEEP_DEBUG_PIN 4
// This will just blink every second
#define BLINKER_PIN 5

// Get this library from http://bleaklow.com/files/2010/Task.tar.gz (and fix WProgram.h -> Arduino.h)
// and read http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html for background and instructions
#include <Task.h>
#include <TaskScheduler.h>

// This defines the variable batterymonitor (a task instance) via extern
#include "batterymonitor.h"

// RGB helpers
#include "gammaramp.h"
#include "rgb.h"

// Animations
#include "animation_tasks.h"
#include "animations.h"
AnimationRunner anim_runner;

#ifdef BRAIN_SERIAL
#include "brain_tasks.h"
EEGReader eeg_reader;
EEGAnimation eeg_anim;
#endif



void load_nth_animation(uint8_t n)
{
    load_animation_to_buffer(&FIRST_ANIMATION);
    uint8_t i = 0;
    while (true)
    {
        if (i == n)
        {
            anim_runner.set_animation(&animation_buffer);
            break;
        }

        if (!animation_buffer.Next)
        {
            break;
        }
        load_animation_to_buffer((const Animation*)animation_buffer.Next);
        i++;
    }
}

class AnimationSwitcher : public TimedTask
{
public:
    // Base methods
    AnimationSwitcher();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
    // My own methods
    virtual void start_cycle();
    virtual void stop_cycle();

private:
    uint8_t current_index;
    AnimationState state;
    
};

AnimationSwitcher::AnimationSwitcher()
: TimedTask(millis())
{
    state = STOPPED;
}

void AnimationSwitcher::start_cycle()
{
    state = RUNNING;
}

void AnimationSwitcher::stop_cycle()
{
    state = STOPPED;
}

bool AnimationSwitcher::canRun(uint32_t now)
{
    if (state == STOPPED)
    {
        return false;
    }
    return TimedTask::canRun(now);
}

void AnimationSwitcher::run(uint32_t now)
{
    incRunTime(2000);
    anim_runner.stop_animation();
    load_nth_animation(current_index);
    current_index++;
    if (current_index >= LAST_ANIMATION)
    {
        current_index = 0;
    }
    anim_runner.start_animation();
}

AnimationSwitcher anim_switcher;


#include "xbee_tasks.h"

void xbee_api_callback(ZBRxResponse rx)
{
    // Check first byte
    switch(rx.getData(0))
    {
        // TODO: Define a sane baseline protocol, for testing we have this dummy RGB setter
        case 0x0:
        {
            setRGB0(rx.getData(1),rx.getData(2),rx.getData(3));
            break;
        }
        case 0x1:
        {
            setRGB1(rx.getData(1),rx.getData(2),rx.getData(3));
            break;
        }
        case 0x2:
        {
            load_nth_animation(rx.getData(1));
            break;
        }
        case 0x3:
        {
            anim_switcher.stop_cycle();
            anim_runner.start_animation();
            break;
        }
        case 0x4:
        {
            anim_runner.stop_animation();
            break;
        }
        case 0x5:
        {
            anim_switcher.stop_cycle();
            load_nth_animation(rx.getData(1));
            anim_runner.start_animation();
            break;
        }
        case 0x58: // Ascii X
        {
            // Your extended protocol goes here
            break;
        }
    }
}
// Init the reader task and set callback
//XBeeRead xbeereader(&xbee_api);
XBeeRead xbeereader;

#include "sleep_task.h"
SleepTask sleeper;

#include "blinker.h"

void high_current_chg(boolean mode)
{
    if (mode)
    {
        pinMode(HIGH_CURRENT_CHG_PIN, OUTPUT);
        digitalWrite(HIGH_CURRENT_CHG_PIN, LOW);
    }
    else
    {
        pinMode(HIGH_CURRENT_CHG_PIN, INPUT);
    }
}


void setup()
{
    // Initialize the 6 PWM with MOSFETs
    for (byte i=16; i < 23; i++)
    {
        pinMode(i, OUTPUT);
    }
    // XBee reset pin, drive low to reset
    pinMode(XBEE_RESET_PIN, OUTPUT);
    reset_xbee();

    // Initialize the XBee wrapper
    Serial2.begin(57600);
    xbee.begin(Serial2);

#ifdef SLEEP_DEBUG_PIN
    pinMode(SLEEP_DEBUG_PIN, OUTPUT);
    // We keep the pin high whenever the sketch is running
    digitalWrite(SLEEP_DEBUG_PIN, HIGH);
#endif
    high_current_chg(true);
}

void loop()
{
    xbeereader.callback = &xbee_api_callback;

    Blinker blinker(BLINKER_PIN, 1000);
    blinker.on_time = 250;
    
    // Start a "demo mode"
#ifndef BRAIN_SERIAL
    anim_switcher.start_cycle();
#endif BRAIN_SERIAL


    // Tasks are in priority order, only one task is run per tick, be sure to keep sleeper as last task if you use it.
     //Task *tasks[] = { &xbeereader, &batterymonitor };
#ifdef BRAIN_SERIAL
    Task *tasks[] = { &xbeereader, &eeg_reader, &eeg_anim, &anim_switcher, &anim_runner, &blinker, &batterymonitor, &sleeper };
#else
    Task *tasks[] = { &xbeereader, &anim_switcher, &anim_runner, &blinker, &batterymonitor, &sleeper };
#endif
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    // Run the scheduler - never returns.
    sched.run();
}
