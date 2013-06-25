#ifndef BRAIN_TASKS_H
#ifndef BRAIN_SERIAL
#error "BRAIN_SERIAL not defined"
#endif
#define BRAIN_TASKS_H

#include "animation_tasks.h"
// Get this from https://github.com/rambo/Arduino-Brain-Library
#include <Brain.h>
#define BRAIN_QUALITY_TH 150

Brain brain(BRAIN_SERIAL);


class EEGAnimation : public AnimationRunner
{
public:
  void new_data();

protected:
    virtual void unpack_frame(const uint8_t *start_of_frame, frame_data& tgt);
};

void EEGAnimation::new_data()
{
    // TODO: what to do ?

    if (state == STOPPED)
    {
        this->start_animation();
    }
}

void EEGAnimation::unpack_frame(const uint8_t *start_of_frame, frame_data& tgt)
{
    return;
}


EEGAnimation eeg_anim;
extern EEGAnimation eeg_anim;


class EEGReader : public Task
{
public:
    EEGReader();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);

protected:
    byte brain_packets;

};

EEGReader::EEGReader()
: Task()
{
    BRAIN_SERIAL.begin(57600);
}

bool EEGReader::canRun(uint32_t now)
{
    return (boolean)BRAIN_SERIAL.available();
}

void EEGReader::run(uint32_t now)
{
    brain_packets = brain.update();
    // No/poor signal
    if (brain.signalQuality > BRAIN_QUALITY_TH)
    {
        eeg_anim.stop_animation();
        // TODO: Check for indicator led pin and turn it on
        return;
    }
    // TODO: if we have indicator led for poor signal turn it off.

    // Packet with the calculated power bands, these we get about once a second
    if (bitRead(brain_packets, 3))
    {
    }

    // Return early, until we actually do something with the raw packets there is no point for wasting time for checking them
    return;
    // Raw packet, these come in at 200Hz, we probably do not want to do anything with them
    if (bitRead(brain_packets, 4))
    {
    }
}

#endif
