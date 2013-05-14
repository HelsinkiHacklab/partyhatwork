// XMEGA 32AU4 Test 1 - Advanced LED blinking on PD5 with clocks:
//
//  - clkCPU/PER = 2 x external 16 MHz crystal oscillator = 32 MHz 
//  - clkPER4 = 4 x clkPER = 128 MHz hi-res clock
// 	- hi-res clock routed to PD4! Don't leave tangling jumpper wire on this pin, because it will act as an antenna!
//  - timer E0 used as system tick counter
//
// This code is demonstration purposes only, do not use in production code!
//
// References are to Atmel's documents:
//  - [AU] = doc8331.pdf, XMEGA AU Manual
//  - [A4U] = doc8387.pdf, ATxmega*A4U Datasheet
// 

#include <stddef.h>					// offsetof(…)

#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/io.h> 				// /usr/local/CrossPack-AVR/avr/include/avr/iox32a4u.h
#include <avr/cpufunc.h> 
#include <avr/interrupt.h>


// If something goes wrong, halt and blink the PD5 LED, frequency somowhat related to error parameter
void HALT(uint8_t error = 0) {
	cli();
	PORTD.DIRSET = 0b00100000; 		
	while (true) {	
		PORTD.OUTTGL = 0b00100000; 	
		for (uint32_t i = 0; i < error*2222ul; i++) { GPIO0 += GPIO1; } 
		PORTD.OUTTGL = 0b00100000; 	
		for (uint32_t i = 0; i < 400000; i++) { GPIO0 += GPIO1; } 
	}
}

// Unhandled interrupts
ISR(BADISR_vect, ISR_BLOCK) { HALT(255); };


// Port E, Counter 0, works as System Tick Counter
volatile uint32_t tick_counter_63to32 = 0;     // Incremented every ~134 second
volatile uint16_t tick_counter_31to16 = 0;     // Incremented ~488 times per second
ISR(TCE0_OVF_vect, ISR_BLOCK) {          		// interrupt service routine that wraps a user defined function supplied by attachInterrupt
	cli();
  		++tick_counter_31to16;
		if (tick_counter_31to16 == 0) { ++tick_counter_63to32;	}
	sei();
}

inline uint16_t getTickCount16() {
  cli();
  	uint16_t   ticks = TCE0.CNT;            // (1a) Read HW timer
  sei();

  return ticks;
}

inline uint32_t getTickCount32() {
  cli();
  
  	uint16_t   timer = TCE0.CNT;            // (1a) Read HW timer
  	if (TCE0.INTFLAGS & 0b1) { 				// INTFLAGS[0] – OVFIF: Overflow/Underflow Interrupt Flag
  		// Handle it straight here, instead of the interrupt handler
    	TCE0.INTFLAGS |= 0b1;                // Clear the pending interrupt
    	timer = TCE0.CNT;                   // (1b) Overflow occured concurrently, read timer again to get new value after overflow
	  	++tick_counter_31to16;				  
		if (tick_counter_31to16 == 0) { ++tick_counter_63to32;	}
  	}
  
  	uint32_t ticks = uint32_t(tick_counter_31to16) << 16 | timer;    // (2) Read volatile overflow counter
  
  sei();

  return ticks;
}

inline uint64_t getTickCount64() {
  cli();
  
  	uint16_t   timer = TCE0.CNT;            // (1a) Read HW timer
  	if (TCE0.INTFLAGS & 0b1) { 				// INTFLAGS[0] – OVFIF: Overflow/Underflow Interrupt Flag
  		// Handle it straight here, instead of the interrupt handler
    	TCE0.INTFLAGS |= 0b1;                // Clear the pending interrupt
    	timer = TCE0.CNT;                   // (1b) Overflow occured concurrently, read timer again to get new value after overflow
	  	++tick_counter_31to16;
		if (tick_counter_31to16 == 0) { ++tick_counter_63to32;	}
  	}
  
  	uint64_t ticks = uint64_t(tick_counter_63to32) << 32 | uint32_t(tick_counter_31to16) << 16 | timer;    // (2) Read volatile overflow counter
  
  sei();

  return ticks;
}

// Assumed clkCPU = 32 MHz
enum {
	TicksInMicrosecond 		= 32ull,
	TicksInMillisecond 		= TicksInMicrosecond*1000,
	TicksInSecond 			= TicksInMillisecond*1000,
	TicksInMinute 			= TicksInSecond*60,
	TicksInHour 			= TicksInMinute*60,
	TicksInDay	 			= TicksInHour*24,
};

// Busyloop delay functions
void delayTicks(uint64_t ticks) {
	uint64_t endtick = getTickCount64() + ticks;
	
	while (endtick > getTickCount64()) { }
}

void delayMicroseconds(uint64_t us) {
	delayTicks(us*TicksInMicrosecond);
}

void delayMilliseconds(uint64_t ms) {
	delayTicks(ms*TicksInMillisecond);
}

void delaySeconds(uint64_t s) {
	delayTicks(s*TicksInSecond);
}




uint8_t ReadCalibrationByte(uint16_t index) {
	uint8_t result;

	// Load the NVM Command register to read the calibration row.
	NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
	result = pgm_read_byte(index);
	
	// Clean up NVM Command register.
	NVM.CMD = NVM_CMD_NO_OPERATION_gc;
	
	return result;
} 

// Sets clkCPU/PER system clock and clkPER4 hi-res
//  - the standard 32 MHz with 128 MHz hi-res = setClockExternal16MHzHiRes(8, 1)
//  - overcloking 68 MHz with 272 MHz hi-res = setClockExternal16MHzHiRes(17, 1) <- not stable, but some chips might work for testing purposes 
void setClockExternal16MHzHiRes(uint8_t multiplier, uint16_t divider) {

	uint8_t pll = 0;
	
	if (1 <= multiplier && multiplier <= 31 ) {
		pll = multiplier;
	} else HALT();
	
	uint8_t prescaler = 0;
		
	switch (divider) {
		case   1 :	prescaler = 0b00000;		break;
		case   2 :	prescaler = 0b00001;		break;
		case   4 :	prescaler = 0b00011;		break;
		case   8 :	prescaler = 0b00101;		break;
		case  16 :	prescaler = 0b00111;		break;
		case  32 :	prescaler = 0b01001;		break;
		case  64 :	prescaler = 0b01011;		break;
		case 128 :	prescaler = 0b01101;		break;
		case 256 :	prescaler = 0b01111;		break;
		case 512 :	prescaler = 0b10001;		break;
		default : 	HALT();					break;	
	}

	OSC.XOSCCTRL = 0b11 << 6 | 0b1011; // Enable 16MHz XTAL with 16K CLK start-up time

	OSC.CTRL |= 0b01000; // Enable External Oscillator
	while((OSC.STATUS & 0b01000) == 0) { } // Wait External Oscillator to stabilize

	OSC.PLLCTRL = 0b11 << 6 | pll; // Select External clock source with 1x..31x PLL
	OSC.CTRL |= 0b10000; // Enable PLL
	while((OSC.STATUS & 0b10000) == 0) { } // Wait PLL to stabilize
	
	uint8_t psctrl = prescaler << 2 | 0b11; // clkPER4 /2 = clkPER2 /2 = clkPER/CPU, protected by CCP!
	CCP = 0xD8; // Unlock Configuration Change Protection
	CLK.PSCTRL = psctrl;
	if (CLK.PSCTRL != psctrl) HALT();		// Error!

	uint8_t ctrl = 0b100; // Select PLL to System Clock
	CCP = 0xD8; // Unlock Configuration Change Protection
	CLK.CTRL = ctrl;
	if (CLK.CTRL != ctrl) HALT();			// Error!

}



int main() {
	cli();

		// Set system clock	
		setClockExternal16MHzHiRes( 8, 1);

		// Timer Counter 0 in port E is used as system cycle counter
		TCE0.CTRLA = 0b0000;													// Stop timer now
		TCE0.CTRLB = 0b0000 << 4 | 0b000 ;										// Normal counter, no output
		TCE0.CTRLD = 0b000 << 5 | 0b0 << 4 | 0b0000;							// No event action, no event source
		TCE0.CTRLE = 0b00;														// Normal counter type 0
		TCE0.INTCTRLA = 0b00 << 2 | 0b11;										// Timer Overflow Interrupt at High-level, no error handling, TODO
		TCE0.INTCTRLB = 0b00 << 6 | 0b00 << 4 | 0b00 << 2 | 0b00;				// No compare nor capture
		TCE0.CNT = 0;															// Clear counter
		TCE0.CTRLA = 0b0001;													// = DIV1, Prescaler: Clk  => Start counter

		// Interrupts and Programmable Multilevel Interrupt Controller
		PMIC.INTPRI = 0; 														// Reset round-robin
		PMIC.CTRL = 0b1 << 7 | 0b0 << 6 | 0b111;   								// Round-robin Scheduling Enabled, Vectors in application flash, section, all levels enabled	
	
	sei();


	// clkPER4 out to PD4
	PORTCFG.CLKEVOUT = 0b1 << 7 | 0b10 << 2 | 0b10;
	PORTD.DIRSET =	0b00010000;

	
	PORTD.DIRSET = 0b00100000; 		// PD4 as output for the LED

	 
	while (true) {	
		
		// Toggle LED
		PORTD.OUTTGL = 0b00100001; 	// Toggle PD4

		delaySeconds(1);

	}

	return -1; // This should never happen	
}

