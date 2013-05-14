// XMEGA 32AU4 Test 3 - Advanced SPI master with DMA demo
//
//	- no interrupts or ping-pong buffering utilized!
//	- assumes many 595-shift registers chained together
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

void dmaRAMtoSPI(DMA_CH_struct & ch, const uint8_t * src, USART_t & dst, uint16_t len, bool repeat = false, bool interrupt = false) {
	while (ch.CTRLA & 0b10000000) { /* waitPerformanceMask |= 0b00000001; */ }
	
	if (interrupt) ch.CTRLB = 0b01; else ch.CTRLB = 0b00;
	if (repeat) ch.REPCNT = 0; else ch.REPCNT = 1;
	
	if (     &dst == &USARTD0) ch.TRIGSRC = DMA_CH_TRIGSRC_USARTD0_DRE_gc;
	else if (&dst == &USARTC1) ch.TRIGSRC = DMA_CH_TRIGSRC_USARTC1_DRE_gc;
	else if (&dst == &USARTD0) ch.TRIGSRC = DMA_CH_TRIGSRC_USARTD0_DRE_gc;
	else if (&dst == &USARTD1) ch.TRIGSRC = DMA_CH_TRIGSRC_USARTD1_DRE_gc;
	else if (&dst == &USARTE0) ch.TRIGSRC = DMA_CH_TRIGSRC_USARTE0_DRE_gc;
	else 					   ch.TRIGSRC = DMA_CH_TRIGSRC_OFF_gc; 
		
	ch.ADDRCTRL = 		0b01010000;;								// scr++, reload after len, dst fixed
	ch.SRCADDR0 =   	uint8_t(((uint16_t)src) & 0xff);
	ch.SRCADDR1 = 		uint8_t(((uint16_t)src) >> 8);
	ch.SRCADDR2 = 		0;
	ch.DESTADDR0 = 		uint8_t(((uint16_t)&dst.DATA) & 0xff);
	ch.DESTADDR1 = 		uint8_t(((uint16_t)&dst.DATA) >> 8);
	ch.DESTADDR2 = 		0;
	ch.TRFCNT = 		len;
	
	ch.CTRLA =			0b10100100;  	// enable, repcnt, single, burst 1 byte -> starts when USART ready
}

void dmaWait(DMA_CH_struct & ch) {
	while (ch.CTRLA & 0b10000000) { /* waitPerformanceMask |= 0b00000010; */ }
}

void dmaStartAgain(DMA_CH_struct & ch) {
	while (ch.CTRLA & 0b10000000) { /* waitPerformanceMask |= 0b00000001; */ }
	
	ch.CTRLA |=		 	0b10000000;  	// enable
}

void dmaTriggerAgain(DMA_CH_struct & ch) {
	while (ch.CTRLA & 0b10000000) { /* waitPerformanceMask |= 0b00000001; */ }
	
	ch.CTRLA |=		 	0b10010000;  	// enable, trigger
}


void waitSPI(USART_t & dst) {
	while (0 == (dst.STATUS & 0b01000000)) { /*waitPerformanceMask |= 0b01000000;*/ } // Wait until transfer complete, TXC
	dst.STATUS = 0b01000000; 														// Clear the TXC
	// TODO, optionally Bit 7 – RXCIF: Receive Complete Interrupt Flag ?
}


void writeSPI(USART_t & dst, uint8_t val) {
	while (0 == (dst.STATUS & 0b00100000)) { /* waitPerformanceMask |= 0b00100000;*/ } 	// Wait until data register ready, DRE
	
	dst.DATA = val;																	// Writing clears the DRE
}

uint8_t readSPI_xx(USART_t & src) {
	writeSPI(src, 0xff);							// Write clock
	return src.DATA;								// Read received data
}

uint8_t readSPI(USART_t & src) {
	while (0 == (src.STATUS & 0b01000000)) { /* waitPerformanceMask |= 0b01000000;*/ } // Wait until transfer complete, TXC
	src.STATUS = 0b11000000; 														// Clear the TXC and RXC!
	if (0 == (src.STATUS & 0b01000000)) { /* waitPerformanceMask |= 0b00001111; */ }			// Then DRE must also be set

	src.DATA = 0xff;																// Writing clears the DRE	

	while (0 == (src.STATUS & 0b10000000)) { /* waitPerformanceMask |= 0b10000000; */ }		// Wait until receive complete, RXC
	
	return src.DATA;																// Reading clears the RXC
}



int main() {
	cli();

		// Set system clock	
		setClockExternal16MHzHiRes( 8, 1);

		DMA.CTRL = 0b10000011;		// Enable DMA controller, no double buffers, priority CH0 > CH1 > CH2 > CH3

		// USARTD0 connected to 595 shift register
		PORTD.OUTSET = 		0b00000001; 		// SS/Latch default to high = not selected
		PORTD.DIRSET = 		0b00000001; 		// SS/Latch drive output
		
		PORTD.OUTCLR = 		0b00000010; 		// CLK default to low
		PORTD.DIRSET = 		0b00000010; 		// CLK drive output
		
		PORTD.DIRCLR = 		0b00000100; 		// MISO sense input
	
		PORTD.DIRSET = 		0b00001000; 		// MOSI/Data drive output
	
	    USARTD0.BAUDCTRLA = 0x00;   			// BSEL=0 (SPI clock = 16MHz) .. BSEL=1 (SPI clock = 8MHz)
	    USARTD0.CTRLC = 	0b11000000;      	// Master SPI mode 0, MSB first
	    USARTD0.CTRLB = 	0b00011000;     	// Enable TX and RX
	    

		// Interrupts and Programmable Multilevel Interrupt Controller
		PMIC.INTPRI = 0; 														// Reset round-robin
		PMIC.CTRL = 0b1 << 7 | 0b0 << 6 | 0b111;   								// Round-robin Scheduling Enabled, Vectors in application flash, section, all levels enabled	
	
	sei();
	
	

	uint8_t buffer[] = { 'H', 'i', 'H', 'i', 'H', 'i', 'H', 'i', 'H', 'i', 'H', 'i', 'H', 'i' };


	while (true) {

		// 0. select slave
		PORTD.OUTCLR = 		0b00000001; 		// SS/Latch low = selected
		
		dmaRAMtoSPI(DMA.CH0, buffer, USARTD0, sizeof(buffer), false, false);
		
		// Do something here..
		
		
		
		
		// 1. wait dma transfer complete
		dmaWait(DMA.CH0);

		// 2. wait last byte actually tranferred
		waitSPI(USARTD0);

		// 3. latch 595s 
		PORTD.OUTSET = 		0b00000001; 		// SS/Latch all bits
	}

	return -1; // This should never happen	
}

