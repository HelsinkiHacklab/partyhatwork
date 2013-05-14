#!/bin/bash

# Break on error
set -e

# Compile and link
avr-gcc -mmcu=atxmega32a4u -I. -Os -Wall main.cpp -o main.elf

# Disassembly
#avr-objdump -Dx main.elf > main.asm 

# Convert progmem from .elf to .hex
avr-objcopy -j .text -j .data -O ihex main.elf main.hex 
# Convert eeprom from .elf to .eep
#avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex main.elf main.eep 

# Program the chip
avrdude -c avrisp2 -P usb -p x32a4 -U flash:w:main.hex
#avrdude -c avrisp2 -P usb -p x32a4 -U flash:w:main.hex -U eeprom:w:main.eep

#dfu-programmer atxmega32a4u flash main.hex
