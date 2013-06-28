#ifndef ANIMATION_TASKS_H
#define ANIMATION_TASKS_H
#include <Arduino.h>
#include <Task.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#define FADE_MIN_TIME 10 //ms


typedef struct {
    const void *Next; // Pointer to next animation
    uint8_t leds; // bits for led 0-7
    uint8_t length; // Frames
    uint8_t modes; // Bit flags, will be later to used to indicate fade mode etc
    const uint8_t *first_frame;
    
} Animation;

Animation animation_buffer;
extern Animation animation_buffer;


void load_animation_to_buffer(const Animation* src)
{
    memcpy_P(&animation_buffer, src, sizeof(Animation));
}
void load_animation_to_buffer(const Animation* src, Animation* tgt)
{
    memcpy_P(tgt, src, sizeof(Animation));
}

enum AnimationState { 
    STOPPED,
    RUNNING,
    FADING_START,
    FADING,
    FADING_END
};

// Pack this matrix to a stuct to make pointer passing saner
typedef struct {
    uint8_t leds[7][3];
    uint16_t wait_ms;
} frame_data;

class AnimationRunner : public TimedTask
{
public:
    // Base methods
    AnimationRunner();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
    // My own methods
    virtual void set_animation(Animation* anim);
    virtual void set_animation(const Animation* anim);
    virtual void start_animation();
    virtual void stop_animation();
    

protected:
    virtual void unpack_frame(const uint8_t *start_of_frame, frame_data& tgt);
    void interpolate_fade();
    void set_leds(frame_data& src);
    void leds_off();


    const Animation* current_animation;
    uint8_t frame_size;
    uint8_t current_step;
    uint8_t num_leds;
    frame_data current_frame;


    frame_data next_frame;
    frame_data fade_frame;
    uint8_t fade_step;
    uint8_t num_fade_steps;

    AnimationState state;
    
};

AnimationRunner::AnimationRunner()
: TimedTask(millis())
{
    state = STOPPED;
}

bool AnimationRunner::canRun(uint32_t now)
{
    if (state == STOPPED)
    {
        return false;
    }
    return TimedTask::canRun(now);
}
/**
 * Frame loader loading from PROGMEM
 */
void AnimationRunner::unpack_frame(const uint8_t *start_of_frame, frame_data& tgt)
{
    uint8_t frame_position = 0;
    for (uint8_t i=0; i < 8; i++)
    {
        if (current_animation->leds & _BV(i))
        {
            tgt.leds[i][0] = pgm_read_byte(start_of_frame+frame_position+0);
            tgt.leds[i][1] = pgm_read_byte(start_of_frame+frame_position+1);
            tgt.leds[i][2] = pgm_read_byte(start_of_frame+frame_position+2);
            frame_position += 3;
        }
    }
    //wait_ms = pgm_read_word(start_of_frame+frame_position);
    tgt.wait_ms = (pgm_read_byte(start_of_frame+frame_position) << 8) + pgm_read_byte(start_of_frame+frame_position+1);
}

/**
 * Load animation struct from SRAM
 */
void AnimationRunner::set_animation(Animation* anim)
{
    leds_off();
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

    this->unpack_frame(current_animation->first_frame, current_frame);
    this->setRunTime(millis());
    state = STOPPED;
}

void AnimationRunner::start_animation()
{
    state = RUNNING;
}

void AnimationRunner::stop_animation()
{
    // Reset the animation
    /** 
     * Seems this fscks some pointer up
    set_animation(current_animation);
     */
    state = STOPPED;
    leds_off();
}

void AnimationRunner::leds_off()
{
    // Clear all leds
    for (uint8_t i=0; i < 8; i++)
    {
        if (current_animation->leds & _BV(i))
        {
            setStdRGB(i, 0x0, 0x0, 0x0);
        }
    }
}

/**
 * Load animation struct from PROGMEM
 */
void AnimationRunner::set_animation(const Animation* anim)
{
    load_animation_to_buffer(anim);
    set_animation(&animation_buffer);
}

void AnimationRunner::interpolate_fade()
{
    for (uint8_t i=0; i < 8; i++)
    {
        if (current_animation->leds & _BV(i))
        {
            for (uint8_t ii=0; ii < 3; ii++)
            {
                uint8_t tmpval1 = current_frame.leds[i][ii];
                uint8_t tmpval2 = next_frame.leds[i][ii];
                if (tmpval1 == tmpval2)
                {
                    fade_frame.leds[i][ii] = tmpval2;
                    continue;
                }
                if (tmpval1 > tmpval2)
                {
                    fade_frame.leds[i][ii] = tmpval1 - (((tmpval1 - tmpval2) / num_fade_steps) * fade_step);
                }
                else
                {
                    fade_frame.leds[i][ii] = (((tmpval2 - tmpval1) / num_fade_steps) * fade_step);
                }
            }
        }
    }
}

void AnimationRunner::set_leds(frame_data& src)
{
    for (uint8_t i=0; i < 8; i++)
    {
        if (current_animation->leds & _BV(i))
        {
            setStdRGB(i, src.leds[i][0], src.leds[i][1], src.leds[i][2]);
        }
    }
}

void AnimationRunner::run(uint32_t now)
{

    switch (state)
    {
        case RUNNING:
        {
            set_leds(current_frame);
          
            if (current_animation->modes == 0x1) // Fade mode animation
            {
                state = FADING_START;
                // Returning without incruntime means we will immediately (well, on next scheduler iteration but that's gonne be real-soon-now) come back to the next state
                return;
            }


            // The default hard-switch handler, load next frame and wait.
            incRunTime(current_frame.wait_ms);
            current_step++;
            if (current_step >= current_animation->length)
            {
                current_step = 0;
            }
            unpack_frame(current_animation->first_frame+(current_step*frame_size), current_frame);

            break;
        }
        // This is porbably unneeded
        case FADING_START:
        {
            // Load next frame (looping as neccessary
            uint8_t next_step = current_step+1;
            if (next_step >= current_animation->length)
            {
                next_step = 0;
            }
            unpack_frame(current_animation->first_frame+(next_step*frame_size), next_frame);
            // Figure out step time and max number of steps we can take within the constraints
            fade_frame.wait_ms = FADE_MIN_TIME;
            uint16_t fade_max_steps = current_frame.wait_ms/FADE_MIN_TIME;
            if (fade_max_steps > 255)
            {
                fade_max_steps = 255;
                fade_frame.wait_ms = current_frame.wait_ms/255;
            }
            // Set next runtime before all the heavy calculcations
            incRunTime(fade_frame.wait_ms);

            // Calculate largest difference between values to interpolate
            uint8_t largest_diff = 0;
            for (uint8_t i=0; i < 8; i++)
            {
                if (current_animation->leds & _BV(i))
                {
                    for (uint8_t ii=0; ii < 3; ii++)
                    {
                        uint8_t tmpval1 = current_frame.leds[i][ii];
                        uint8_t tmpval2 = next_frame.leds[i][ii];
                        uint8_t tmpdiff;
                        if (tmpval1 > tmpval2)
                        {
                            tmpdiff = tmpval1-tmpval2;
                        }
                        else
                        {
                            tmpdiff = tmpval2-tmpval1;
                        }
                        if (tmpdiff > largest_diff)
                        {
                            largest_diff = tmpdiff;
                        }
                    }
                }
            }
            // Set the number of steps we are going to fade for
            if (largest_diff <= fade_max_steps)
            {
                num_fade_steps = largest_diff;
            }
            else
            {
                num_fade_steps = fade_max_steps;
            }

            fade_step = 1;
            interpolate_fade();
            state = FADING;
            break;
        }
        case FADING:
        {
            set_leds(fade_frame);

            // Set next runtime before all the heavy calculcations
            incRunTime(fade_frame.wait_ms);
            if (fade_step >= num_fade_steps)
            {
                state = FADING_END;
            }
            fade_step++;
            interpolate_fade();

            break;
        }
        case FADING_END:
        {
            set_leds(fade_frame);
            
            incRunTime(fade_frame.wait_ms);
            state = RUNNING;

            current_step++;
            if (current_step >= current_animation->length)
            {
                current_step = 0;
            }
            //unpack_frame(current_animation->first_frame+(current_step*frame_size), leds, &wait_ms);
            // We already unpacked the next frame to calculate the interpolations, so just switch them over.
            current_frame = next_frame;
            
            break;
        }
    }


}

AnimationRunner anim_runner;
extern AnimationRunner anim_runner;

#endif
