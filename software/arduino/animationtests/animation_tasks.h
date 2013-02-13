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
    // Base methods
    AnimationRunner();
    virtual void run(uint32_t now);
    // My own methods
    virtual void set_animation(Animation* anim);
    

private:
    Animation* current_animation;
    uint8_t frame_size;
    uint8_t current_step;
};

AnimationRunner::AnimationRunner()
: TimedTask(millis())
{
}


void AnimationRunner::set_animation(Animation* anim)
{
    current_step = 0;
    current_animation = anim;
    uint8_t num_leds = 0;
    for (uint8_t i=0; i < 8; i++)
    {
        if (current_animation->leds & _BV(i))
        {
            num_leds++;
        }
    }
    frame_size = (num_leds*3)+2;
    
    
    
    Serial.println(F("Animation switched"));
    Serial.print(F("leds=0x"));
    Serial.println(current_animation->leds, HEX);
    Serial.print(F("length="));
    Serial.println(current_animation->length, DEC);
    Serial.print(F("first_frame (address)=0x"));
    Serial.println((uint16_t)&current_animation->first_frame, HEX);

    Serial.print(F("num_leds="));
    Serial.println(num_leds, DEC);
    Serial.print(F("frame_size="));
    Serial.println(frame_size, DEC);

}



void AnimationRunner::run(uint32_t now)
{
    // TODO implement
}


#endif
