// XMEGA 32AU4 Test 2 - Basic serial demo with USARTD0 with RxD = PD2 and TxD = PD3 
//
//  - busyloopped implementation, no interrupts or dma utilized!
//  - 115200N8
//	- echoes back uppercase
//		- sudo screen -U /dev/tty.usbserial-A900J2PQ 115200
//		- ctrl+a k y
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
		default : 	HALT();						break;	
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

// Send one byte over serial
void putc(uint8_t c) {
	while (0 == (USARTD0.STATUS & 0b00100000)) { } 	// Wait while Data Register Empty 
	USARTD0.DATA = c; 								// This will clear the flag
}

// Wait and read one byte over serial
uint8_t getc() {
	while (0 == (USARTD0.STATUS & 0b10000000)) { } 	// Wait while Receive Complete
	return USARTD0.DATA;							// This will clear the flag
}

// Wait for last serial byte has actually send over wire
void waittx() {
	while (0 == (USARTD0.STATUS & 0b01000000)) { }  // Wait while Transmit Complete
	USARTD0.STATUS = 0b01000000;					// Clear the flag
}

// Print null terminated string to serial
void debugPrint(const char * str) {
	while (*str) putc(uint8_t(*str++));
}

// Print hexadeciman number to serial port
void debugPrintHex(uint64_t value, uint8_t digits) {
	uint8_t buffer[16] = { 0 };
    uint8_t d = digits;

	while(value && d) {
		uint8_t digit = value & 0b1111; 
		value >>= 4;
		buffer[--d] = digit;
		
	}
	
	for (uint8_t i = 0; i < digits; ++i) {
		uint8_t digit = buffer[i];
		if (digit > 9) {
			putc(digit + 'A' - 10);
		} else {
			putc(digit + '0');
		}
			
	}
}


// Print deciman number to serial port
void debugPrintDec(uint64_t value, uint8_t digits) {
	uint8_t buffer[20] = { 0 };
    uint8_t d = digits;

	while(value && d) {
		uint8_t digit = value%10; 
		value /= 10;
		buffer[--d] = digit;
		
	}
	
	for (uint8_t i = 0; i < digits; ++i) {
		putc(buffer[i] + '0');
	}
}

// Print binary number to serial port
void debugPrintBin(uint8_t value, uint8_t digits) {
	uint8_t buffer[8] = { 0 };
    uint8_t d = digits;

	while(value && d) {
		uint8_t digit = value%2; 
		value /= 2;
		buffer[--d] = digit;
		
	}
	
	for (uint8_t i = 0; i < digits; ++i) {
		putc(buffer[i] + '0');
	}
}




int main() {
	cli();

		// Set system clock	
		setClockExternal16MHzHiRes( 8, 1);

		// Set USARTD0 to standard serial
		PORTD.OUTSET = 0b00001000;					// Set TxD pin high
		PORTD.DIRSET = 0b00001000;					// Set TxD pin as output
		PORTD.DIRCLR = 0b00000100;					// Set RxD pin as input

		USARTD0.CTRLA = 0;  					// No interrupts
		
		USARTD0.CTRLC = 0b00 << 6 | 0b00 << 4 | 0b0 << 3 | 0b011;	// Asynchronous, No parity, 1 Stop Bit, 8-bit Character Size
		// uint16_t	baudrate = 115200
		USARTD0.BAUDCTRLA = 131;				// 131
		USARTD0.BAUDCTRLB = 0b1101 << 4;			// -3

		// uint16_t	baudrate = 230400
		//USARTD0.BAUDCTRLA = 123;	
		//USARTD0.BAUDCTRLB = 0b1100 << 4;			// -4
	
		USARTD0.CTRLB = 0b1 << 4 | 0b1 << 3;   					// Receiver Enable, Transmitter Enable

		// Interrupts and Programmable Multilevel Interrupt Controller
		PMIC.INTPRI = 0; 														// Reset round-robin
		PMIC.CTRL = 0b1 << 7 | 0b0 << 6 | 0b111;   								// Round-robin Scheduling Enabled, Vectors in application flash, section, all levels enabled	
	
	sei();

	debugPrint("Hello World!");
	debugPrint("\r\n");
	 
	while (true) {
			
		uint8_t c = getc();
		
		if (32 <= c && c < 128) {
			debugPrint("You just pressed ASCII ");
			debugPrintDec(c, 3);
			debugPrint("\r\n");
		} else {
			debugPrint("0b");
			debugPrintBin(c, 8);
			debugPrint("\r\n");
		}

	}

	return -1; // This should never happen	
}

