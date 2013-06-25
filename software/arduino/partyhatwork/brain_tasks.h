#ifndef BRAIN_TASKS_H
#ifndef BRAIN_SERIAL
#error "BRAIN_SERIAL not defined"
#endif
#define BRAIN_TASKS_H

#include "animation_tasks.h"

class EEGAnimation : public AnimationRunner
{
};

class EEGReader : public Task
{
public:
    EEGReader();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
};

EEGReader::EEGReader()
: Task()
{
}

bool EEGReader::canRun(uint32_t now)
{
    return (boolean)BRAIN_SERIAL.available();
}

void EEGReader::run(uint32_t now)
{
  
}

#endif
