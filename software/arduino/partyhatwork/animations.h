#ifndef ANIMATIONS_H
#define ANIMATIONS_H
#include "animation_tasks.h"

/**
 * Usually it's a good idea to add new animations to end of file and then point the next to the previously last 
 * and then define this as the last one.
 *
 * This is the simplest way to make sure the linked list stays so.
 */
#define FIRST_ANIMATION strobe_anim 
//#define FIRST_ANIMATION police3_anim 

const uint8_t police1_frames[] PROGMEM = { 
  0xff, 0x0, 0x0,    0x0, 0x0, 0x0,     0x1, 0xf4,
  0x0,  0x0, 0x0,    0x0, 0x0, 0xff,    0x1, 0xf4,
  0xff, 0x0, 0x0,    0x0, 0x0, 0x0,     0x1, 0xf4,
  0x0,  0x0, 0x0,    0x0, 0x0, 0xff,    0x1, 0xf4,
};

const Animation police1_anim PROGMEM = {
    0x0,
    0x3, // leds 0 & 1
    0x4,
    0x1,
    police1_frames  // Array name is always a pointer so no need for &
};

const uint8_t police2_frames[] PROGMEM = { 
  0xff, 0x0, 0x0,    0x0, 0x0, 0x0,     0x1, 0xf4,
  0x0,  0x0, 0x0,    0x0, 0x0, 0xff,    0x1, 0xf4,
  0x0,  0x0, 0xff,   0x0, 0x0, 0x0,     0x1, 0xf4,
  0x0,  0x0, 0x0,    0xff, 0x0, 0x0,    0x1, 0xf4,
};

const Animation police2_anim PROGMEM = {
    &police1_anim,
    0x3, // leds 0 & 1
    0x4,
    0x1,
    police2_frames  // Array name is always a pointer so no need for &
};

const uint8_t police3_frames[] PROGMEM = { 
  0xff, 0x0, 0x0,    0x0, 0x0, 0xff,     0x1, 0xf4,
  0x0,  0x0, 0xff,   0xff, 0x0, 0x0,     0x1, 0xf4,
};

const Animation police3_anim PROGMEM = {
    &police2_anim,
    0x3, // leds 0 & 1
    0x2,
    0x1,
    police3_frames  // Array name is always a pointer so no need for &
};

const uint8_t strobe_frames[] PROGMEM = {
    0xff, 0xff, 0xff,   0xff, 0xff, 0xff,   0x0, 0xa,
    0x00, 0x00, 0x00,   0x00, 0x00, 0x00,   0x0, 0x5a,
};
const Animation strobe_anim PROGMEM = {
    &police3_anim,
    B00000011,
    2,
    0x0,
    strobe_frames,
};

// ***** KEEP THIS FUNCTION AS LAST PART OF FILE, ADD ANIMATION ABOVE *****

uint8_t count_animations()
{
    uint8_t i = 0;
    load_animation_to_buffer(&FIRST_ANIMATION);
    while (true)
    {
        if (!animation_buffer.Next)
        {
            break;
        }
        load_animation_to_buffer((const Animation*)animation_buffer.Next);
        i++;
    }
    return i;
}
extern const uint8_t LAST_ANIMATION = count_animations();

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

#endif

