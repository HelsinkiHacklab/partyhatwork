// XMEGA 32AU4 Test 0 - Basic LED blinking on PD5 with default internal 2 MHz oscillator
//
// This code is demonstration purposes only, do not use in production code!
//
// References are to Atmel's documents:
//  - [AU] = doc8331.pdf, XMEGA AU Manual
//  - [A4U] = doc8387.pdf, ATxmega*A4U Datasheet
// 

#include <avr/io.h> 				// .../avr/include/avr/iox32a4u.h


int main() {
	
	// Set PD0 as output, it has LED in the default board 
	// - there are different registers for setting=SET, clearing=CLR and toggling=TGL for only some bits in direction=DIR and output=OUT registers
	// - for more info see [AU 13.13]
	PORTD.DIRSET = 0b00100000; 		


	// Stay in infinite loop
	while (true) {	
			
		// Toggle PD0 pin	
		PORTD.OUTTGL = 0b00100000; 	
		
		// Approximate 1 second delay by readind and writing the general purpose I/O register so that compiler can't optimize it away
		for (uint32_t i = 0; i < 222222; i++) { GPIO0 += GPIO1; } 
	}

	return -1; // This should never happen	
}

