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

From serial_init.cpp: RX/TX
SERIAL_DEFINE(Serial, C, 0); -> PC1/PC3 == 17/19
SERIAL_DEFINE(Serial1, C, 1); -> PC6/PC7 == 22/23
SERIAL_DEFINE(XBEE_SERIAL, D, 0); -> PD2/PD3 == 26/27
//SERIAL_DEFINE(Serial3, D, 1);  This is USB
SERIAL_DEFINE(Serial3, E, 0); -> PE2/PE3 == 2/3

 * 
 */

// If you have EEG module connected, connect it here.
/*
#define BRAIN_SERIAL Serial1
#define BRAIN_POWER_PIN 6
*/
// The high and low parts of the xbee address where to report brain activity, this will be passed to XBeeAddress64-constructor
// TODO: This should be readable (since it's the coordinator) via the XBee API somehow I'm sure...
#define BRAIN_REPORT_TO 0x0013a200, 0x403276df
#define BAT_SENSE_PIN A9
#define HIGH_CURRENT_CHG_PIN 12
#define XBEE_RESET_PIN 28
// This will be HIGH whenever we are *not* sleeping
#define SLEEP_DEBUG_PIN 4
// This will just blink every second
#define BLINKER_PIN 5

// On the partyhat board this is where the XBee is hardwired to
#define XBEE_SERIAL Serial2

// Get this library from http://bleaklow.com/files/2010/Task.tar.gz (and fix WProgram.h -> Arduino.h)
// and read http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html for background and instructions
#include <Task.h>
#include <TaskScheduler.h>

// Get this library from http://code.google.com/p/xbee-arduino/
#include <XBee.h>
#include "xbee_tasks.h"

// This defines the variable batterymonitor (a task instance) via extern
#include "batterymonitor.h"

// RGB helpers
#include "gammaramp.h"
#include "rgb.h"

// Animations (define some task instances via extern and a bunch of helper functions)
#include "animation_tasks.h"
#include "animations.h"
#include "demo_tasks.h"

#ifdef BRAIN_SERIAL
// Get this from https://github.com/rambo/Arduino-Brain-Library
#include <Brain.h>
// Get this from https://github.com/ratkins/RGBConverter
#include <RGBConverter.h>
#include "brain_tasks.h"
#endif

// This implements the XBee API, first byte is command identifier rest of them are command specific.
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
#ifdef BRAIN_SERIAL
            eeg_reader.enable(false);
#endif
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
#ifdef BRAIN_SERIAL
            eeg_reader.enable(false);
#endif
            anim_switcher.stop_cycle();
            load_nth_animation(rx.getData(1));
            anim_runner.start_animation();
            break;
        }
#ifdef BRAIN_SERIAL
        case 0xF:
        {
            anim_runner.stop_animation();
            anim_switcher.stop_cycle();
            eeg_reader.enable((boolean)rx.getData(1));
            break;
        }
#endif
        case 0x58: // Ascii X
        {
            // Your extended protocol goes here
            break;
        }
    }
}

// An idle task to conserve a little bit of power (it's pretty meaningless as long as we have leds turned on but every little thing counts sometimes and this was easy to do)
#include "sleep_task.h"

#ifdef BLINKER_PIN
#include "blinker.h"
#endif


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
    XBEE_SERIAL.begin(57600);
    xbee.begin(XBEE_SERIAL);

#ifdef BRAIN_SERIAL
    BRAIN_SERIAL.begin(57600);
#endif


#ifdef SLEEP_DEBUG_PIN
    pinMode(SLEEP_DEBUG_PIN, OUTPUT);
    // We keep the pin high whenever the sketch is running
    digitalWrite(SLEEP_DEBUG_PIN, HIGH);
#endif
    // Enable high-current charging on the charge controller by default, to be exact we should negotiate for it (or test for dumb charger) first.
    high_current_chg(true);
}

void loop()
{
    // Add the XBee API callback function pointer to the task
    xbeereader.callback = &xbee_api_callback;

#ifdef BLINKER_PIN
    // Initialize a blinker used to make sure the task system is running
    Blinker blinker(BLINKER_PIN, 1000);
    blinker.on_time = 250;
#endif
    
    // Start a "demo mode" (if there is no EEG reader) that will simply cycle the available animations
#ifndef BRAIN_SERIAL
    anim_switcher.start_cycle();
#else
    eeg_reader.enable(true);
#endif

    // Tasks are in priority order, only one task is run per tick, be sure to keep sleeper as last task if you use it.
    Task *tasks[] = { 
        &xbeereader,
        &anim_runner,
        &anim_switcher,
#ifdef BRAIN_SERIAL
        &eeg_anim,
        &eeg_reader,
#endif
#ifdef BLINKER_PIN
        &blinker,
#endif
        &batterymonitor,
        &sleeper
    };
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    // Run the scheduler - never returns.
    sched.run();
}
