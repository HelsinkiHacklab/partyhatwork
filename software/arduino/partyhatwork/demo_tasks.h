#ifndef DEMO_TASKS_H
#define DEMO_TASKS_H
#include <Arduino.h>
#include <Task.h>
#include "animation_tasks.h"
#include "animations.h"

class AnimationSwitcher : public TimedTask
{
public:
    // Base methods
    AnimationSwitcher();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
    // My own methods
    virtual void start_cycle();
    virtual void stop_cycle();

private:
    uint8_t current_index;
    AnimationState state;
    
};

AnimationSwitcher::AnimationSwitcher()
: TimedTask(millis())
{
    state = STOPPED;
}

void AnimationSwitcher::start_cycle()
{
    state = RUNNING;
}

void AnimationSwitcher::stop_cycle()
{
    state = STOPPED;
}

bool AnimationSwitcher::canRun(uint32_t now)
{
    if (state == STOPPED)
    {
        return false;
    }
    return TimedTask::canRun(now);
}

void AnimationSwitcher::run(uint32_t now)
{
    incRunTime(2000);
    anim_runner.stop_animation();
    load_nth_animation(current_index);
    current_index++;
    if (current_index >= LAST_ANIMATION)
    {
        current_index = 0;
    }
    anim_runner.start_animation();
}

AnimationSwitcher anim_switcher;
extern AnimationSwitcher anim_switcher;

#endif

