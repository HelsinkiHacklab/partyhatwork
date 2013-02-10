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

#define BAT_SENSE_PIN A9
#define HIGH_CURRENT_CHG_PIN 12
#define XBEE_RESET_PIN 28

// Get this library from http://bleaklow.com/files/2010/Task.tar.gz (and fix WProgram.h -> Arduino.h)
// and read http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html for background and instructions
#include <Task.h>
#include <TaskScheduler.h>

/*
#include "batterymonitor.h"
BatteryMonitor batterymonitor(BAT_SENSE_PIN);
*/

// RGB helpers
#include "gammaramp.h"
#include "rgb.h"

// Get this library from http://code.google.com/p/xbee-arduino/
#include <XBee.h>
#include "xbee_tasks.h"

void xbee_api(ZBRxResponse rx)
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

}

void loop()
{
    xbeereader.callback = &xbee_api;

    // Tasks are in priority order, only one task is run per tick
    //Task *tasks[] = { &batterymonitor, };
    //Task *tasks[] = { &xbeereader, &batterymonitor, };
    Task *tasks[] = { &xbeereader, };
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    // Run the scheduler - never returns.
    sched.run();
}
