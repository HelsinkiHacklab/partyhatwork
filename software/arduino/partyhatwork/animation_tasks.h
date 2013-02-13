#ifndef ANIMATION_TASKS_H
#define ANIMATION_TASKS_H
#include <Arduino.h>
#include <Task.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

typedef struct {
    void *Next; // Pointer to next animation
    uint8_t leds; // bits for led 0-7
    uint8_t length; // Frames
    uint8_t modes; // Bit flags, will be later to used to indicate fade mode etc
    uint16_t first_frame;
    
} Animation_PGM PROGMEM;


#endif
