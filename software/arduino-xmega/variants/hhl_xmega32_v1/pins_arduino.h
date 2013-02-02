/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

*/

/**
 * Pin map for https://github.com/HelsinkiHacklab/XMega-boards/blob/master/XmegaA4U_v1.brd
 * 
 *

            USB
    1   PE0     GND
    2   PE1     GND
    3   PE2     PD5   30
    4   PE3     PD4   29
    5   PA0     PD3   28
    6   PA1     PD2   27
    7   PA2     PD1   26
    8   PA3     PD0   25
    9   PA4     PC7   24
   10   PA5     PC6   23
   11   PA6     PC5   22
   12   PA7     PC4   21
   13   PB0     PC3   20
   14   PB1     PC2   19
   15   PB2     PC1   18
   16   PB3     PC0   17

Port A&B is ADC, ANALOG0 -> PB3 ANALOG7 -> PA11

USARTS

  Serial = 19 (RX) & 20 (TX)
  Serial1 = 23 (RX) & 24 (TX)
  Serial2 = 27 (RX) & 28 (TX)
  Serial3 = 3 (RX) & 4 (TX)

*/


#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>


#define REPEAT8(x) x, x, x, x, x, x, x, x
#define BV0TO7 _BV(0), _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7)
#define BV7TO0 _BV(7), _BV(6), _BV(5), _BV(4), _BV(3), _BV(2), _BV(1), _BV(0)

#define NUM_DIGITAL_PINS            30
#define NUM_ANALOG_INPUTS           8
#define EXTERNAL_NUM_INTERRUPTS     36 // PORTA to PORTE 3

#define analogInputToDigitalPin(p)  ((p < 6) ? (p) + 22 : -1)
#define digitalPinHasPWM(p)         (((p) >= 2 && (p) <= 6) || ((p) >= 9 && (p)<= 11) || ((p) >= 14 && (p)<= 19))


// TODO: Map correctly
static const uint8_t SS    = 6;
static const uint8_t MOSI  = 11;
static const uint8_t MISO  = 12;
static const uint8_t SCK   = 13;

// TODO: Map correctly
static const uint8_t SDA = 2;
static const uint8_t SCL = 3;
static const uint8_t LED_BUILTIN = 13;

static const uint8_t A0 = 5;
static const uint8_t A1 = 6;
static const uint8_t A2 = 7;
static const uint8_t A3 = 8;
static const uint8_t A4 = 9;
static const uint8_t A5 = 10;
static const uint8_t A6 = 11;
static const uint8_t A7 = 12;
static const uint8_t A8 = 14;
static const uint8_t A9 = 14;
static const uint8_t A10 = 15;
static const uint8_t A11 = 16;

#define Wire xmWireC
#define Wire1 xmWireE

#define SPI_PORT	SPID

#define digitalPinToPCICR(p)    (((p) >= 0 && (p) <= 21) ? (&PCICR) : ((uint8_t *)0))
#define digitalPinToPCICRbit(p) (((p) <= 7) ? 2 : (((p) <= 13) ? 0 : 1))
#define digitalPinToPCMSK(p)    (((p) <= 7) ? (&PCMSK2) : (((p) <= 13) ? (&PCMSK0) : (((p) <= 21) ? (&PCMSK1) : ((uint8_t *)0))))
#define digitalPinToPCMSKbit(p) (((p) <= 7) ? (p) : (((p) <= 13) ? ((p) - 8) : ((p) - 14)))

#ifdef ARDUINO_MAIN

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PORTA,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
	(uint16_t) &PORTE,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
    PE,
    PE,
    PE,
    PE,
    PA,
    PA,
    PA,
    PA,
    PA,
    PA,
    PA,
    PA,
    PB,
    PB,
    PB,
    PB,
    PC,
    PC,
    PC,
    PC,
    PC,
    PC,
    PC,
    PC,
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
		// PIN IN PORT		
		// -------------------------------------------		
		_BV( 0 ), 
		_BV( 1 ),
		_BV( 2 ),
		_BV( 3 ),
		_BV( 0 ),
		_BV( 1 ),
		_BV( 2 ),
		_BV( 3 ),
		_BV( 4 ), 
		_BV( 5 ),
		_BV( 6 ),
		_BV( 7 ),
		_BV( 0 ),
		_BV( 1 ),
		_BV( 2 ), 
		_BV( 3 ),
		_BV( 0 ),
		_BV( 1 ),
		_BV( 2 ),
		_BV( 3 ),
		_BV( 4 ),
		_BV( 5 ), 
		_BV( 6 ), 
		_BV( 7 ),
		_BV( 0 ),
		_BV( 1 ),
		_BV( 2 ),
		_BV( 3 ),
		_BV( 4 ),
		_BV( 5 ),
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	// TIMERS		
	// -------------------------------------------		
	TIMER_E0A   	, 
	TIMER_E0B   	, 
	TIMER_E0C   	, 
	TIMER_E0D   	,   // 5
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, // 13
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	TIMER_C0A   	,  // 17 
	TIMER_C0B   	, 
	TIMER_C0C   	, 
	TIMER_C0D   	, 
	TIMER_C1A   	, 
	TIMER_C1B   	, 
	NOT_ON_TIMER	, 
	NOT_ON_TIMER	, 
	TIMER_D0A       ,  // 25
	TIMER_D0B       , 
	TIMER_D0C       , 
	TIMER_D0D       , 
	TIMER_D1A       , 
	TIMER_D1B       , 
};

// TODO: Map correctly
const TC0_t* PROGMEM timer_to_tc0_PGM[] = {
	NULL,

	&TCC0,
	&TCC0,
	&TCC0,
	&TCC0,
	NULL,
	NULL,

	&TCD0,
	&TCD0,
	&TCD0,
	&TCD0,
	NULL,
	NULL,

	&TCE0,
	&TCE0,
	&TCE0,
	&TCE0,
	NULL,
	NULL,
	};

// TODO: Map correctly
const TC1_t* PROGMEM timer_to_tc1_PGM[] = {
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,
	&TCC1,
	&TCC1,

	NULL,
	NULL,
	NULL,
	NULL,
	&TCD1,
	&TCD1,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

// TODO: Map correctly
const uint8_t PROGMEM timer_to_channel_PGM[] = {
	NOT_ON_TIMER,

    0,
    1,
    2,
    3,
    0,
    1,

    0,
    1,
    2,
    3,
    0,
    1,

    0,
    1,
    2,
    3,
    0,
    1,
};

// TODO: Map correctly
const uint8_t PROGMEM adc_to_channel_PGM[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
};

#endif

#endif
