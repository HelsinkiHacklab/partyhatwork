#ifndef BRAIN_TASKS_H
#ifndef BRAIN_SERIAL
#error "BRAIN_SERIAL not defined"
#endif
#define BRAIN_TASKS_H
#include <Arduino.h>
#include <Task.h>


#include "animation_tasks.h"
// Get this from https://github.com/rambo/Arduino-Brain-Library
#include <Brain.h>
#define BRAIN_QUALITY_TH 150

#ifdef BRAIN_REPORT_TO
#include <XBee.h>
#include "xbee_tasks.h"
uint8_t eeg_payload[] = {
    0x42, // 0x42 = "B", to identify this as brain packet, 
    0x0, // Quality
    0x0, // Attention
    0x0, // Meditation
    0x0, 0x0, 0x0, // The 8 power bands, these are 24bit unsigned integers: delta, theta, low-alpha high-alpha, low-beta, high-beta, low-gamma, and mid-gamma
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
}; 
// SH + SL Address of receiving XBee
XBeeAddress64 coordinator_addr64 = XBeeAddress64(BRAIN_REPORT_TO);
ZBTxRequest zb_EEG_Tx = ZBTxRequest(coordinator_addr64, eeg_payload, sizeof(eeg_payload));
#endif

Brain brain(BRAIN_SERIAL);

Animation eeg_animation_muckable = {
    0x0,
    B000011,
    2,
    0x0
};

class EEGAnimation : public AnimationRunner
{
public:
    EEGAnimation();
    void new_data();

protected:
    virtual void unpack_frame(const uint8_t *start_of_frame, frame_data& tgt);
    frame_data eeg_frame;
    
};

EEGAnimation::EEGAnimation()
{
    set_animation(&eeg_animation_muckable);
}


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

#ifdef BRAIN_REPORT_TO
unsigned long last_report_time;
const unsigned int report_interval = 3000;
#endif

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

#ifdef BRAIN_REPORT_TO
        // Report low quality at intervals
        if ((millis() - last_report_time) > report_interval)
        {
            eeg_payload[1] =  brain.signalQuality;
            xbee.send(zb_EEG_Tx);
            last_report_time = millis();
        }
#endif        
        return;
    }
    // TODO: if we have indicator led for poor signal turn it off.

    // Packet with the calculated power bands, these we get about once a second
    if (bitRead(brain_packets, 3))
    {
#ifdef BRAIN_REPORT_TO
        eeg_payload[1] =  brain.signalQuality;
        eeg_payload[2] =  brain.attention;
        eeg_payload[3] =  brain.meditation;
        for(byte j = 0; j < EEG_POWER_BANDS; j++)
        {
            // Split the long (but they contain only 24bits of data) back to the individual bytes
            eeg_payload[(4+j)] = (uint8_t)(brain.eegPower[j] >> 16);
            eeg_payload[(5+j)] = (uint8_t)((brain.eegPower[j] & 0xffff) >> 8);
            eeg_payload[(6+j)] = (uint8_t)((brain.eegPower[j] & 0xff));
        }
        // Fire and forget... (the reader task will eventually get the response for this but we do not care since there is nothing we can do about it)
        xbee.send(zb_EEG_Tx);
#endif
        eeg_anim.new_data();
    }

    // Return early, until we actually do something with the raw packets there is no point for wasting time for checking them
    return;
    // Raw packet, these come in at 200Hz, we probably do not want to do anything with them
    if (bitRead(brain_packets, 4))
    {
    }
}

EEGReader eeg_reader;
extern EEGReader eeg_reader;


#endif
