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
    uint8_t *first_frame;
    
} Animation;



class AnimationRunner : public TimedTask
{
public:
    // Base methods
    AnimationRunner();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
    // My own methods
    virtual void set_animation(Animation* anim);
    
    // public variables
    boolean running;

private:
    virtual void unpack_frame(uint8_t *start_of_frame);


    uint8_t leds[7][3];

    Animation* current_animation;
    uint8_t frame_size;
    uint8_t current_step;
    uint8_t num_leds;
    uint16_t wait_ms;

};

AnimationRunner::AnimationRunner()
: TimedTask(millis())
{
}

bool AnimationRunner::canRun(uint32_t now)
{
    return running;
}

void AnimationRunner::unpack_frame(uint8_t *start_of_frame)
{
    uint8_t frame_position = 0;
    for (uint8_t i=0; i < 8; i++)
    {
        if (current_animation->leds & _BV(i))
        {
            leds[i][0] = start_of_frame[frame_position+0];
            leds[i][1] = start_of_frame[frame_position+1];
            leds[i][2] = start_of_frame[frame_position+2];
            frame_position += 3;
        }
    }
    wait_ms = (start_of_frame[frame_position] << 9) + start_of_frame[frame_position+1];

    for (uint8_t i=0; i < 8; i++)
    {
        if (current_animation->leds & _BV(i))
        {
            Serial.print(F("LED"));
            Serial.print(i, DEC);
            Serial.print(F(" values:"));
            Serial.print(F(" 0x"));
            Serial.print(leds[i][0], HEX);
            Serial.print(F(" 0x"));
            Serial.print(leds[i][1], HEX);
            Serial.print(F(" 0x"));
            Serial.println(leds[i][2], HEX);
        }
    }
    Serial.print(F("wait_time="));
    Serial.println(wait_ms, DEC);
    
}

void AnimationRunner::set_animation(Animation* anim)
{
    current_step = 0;
    current_animation = anim;
    num_leds = 0;
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

    this->unpack_frame(current_animation->first_frame);

    running = true;
}



void AnimationRunner::run(uint32_t now)
{
    Serial.print(F("Running step "));
    Serial.println(current_step, DEC);

    // TODO: Set the LED values

    incRunTime(wait_ms);
    current_step++;
    if (current_step > current_animation->length)
    {
        current_step = 0;
    }
    unpack_frame(current_animation->first_frame+(current_step*frame_size));
}


#endif
