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
    void *first_frame;
    
} Animation;



class AnimationRunner : public TimedTask
{
public:
    // Create a new blinker for the specified pin and rate.
    AnimationRunner();
    virtual void run(uint32_t now);
//    Animation* current_animation;
    

private:
    uint8_t frame_size;
    uint8_t current_step;
};

void AnimationRunner::run(uint32_t now)
{
    // TODO implement
}


#endif
