#ifndef BRAIN_TASKS_H
#ifndef BRAIN_SERIAL
#error "BRAIN_SERIAL not defined"
#endif
#define BRAIN_TASKS_H
#include <Arduino.h>
#include <Task.h>
#include <RGBConverter.h>
RGBConverter rgbconverter;

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
    0x0 // The bitfield showing which data was decoded
}; 
// SH + SL Address of receiving XBee
XBeeAddress64 coordinator_addr64 = XBeeAddress64(BRAIN_REPORT_TO);
ZBTxRequest zb_EEG_Tx = ZBTxRequest(coordinator_addr64, eeg_payload, sizeof(eeg_payload));
#endif

Brain brain(BRAIN_SERIAL);
byte brain_packets;


Animation eeg_animation_muckable = {
    0x0, // There is no next animation in the linked list, this is not part of the animations chain
    0x3, // leds 0 & 1
    0x2, // Two frames long (not that matters since we overload the unpack_frame method
    0x1, // Fading
    0x0 // There is no address for the first frame of the animation (we overload the unpacking function to cope with this)
};

// Hues for the 8 bands, right now they're just spread evenly over the space but hand-picking might be an option as well
const double eeg_band_hsv_hue[EEG_POWER_BANDS] = {
    0.0, // delta, 
    (double)1.0/(double)8.0, // theta
    (double)1.0/(double)8.0*(double)2.0, // low-alpha 
    (double)1.0/(double)8.0*(double)3.0, // high-alpha, 
    (double)1.0/(double)8.0*(double)4.0, // low-beta 
    (double)1.0/(double)8.0*(double)5.0, // high-beta
    (double)1.0/(double)8.0*(double)6.0, // low-gamma
    (double)1.0/(double)8.0*(double)7.0  // mid-gamma
};

class EEGAnimation : public AnimationRunner
{
public:
    EEGAnimation();
    virtual void new_data();

protected:
    virtual void unpack_frame(const uint8_t *start_of_frame, frame_data& tgt);
    frame_data eeg_frame;
    
};

EEGAnimation::EEGAnimation()
{
    this->set_animation(&eeg_animation_muckable);
    this->start_animation();
}


void EEGAnimation::new_data()
{
    // Find strongest band
    unsigned long strongest_value;
    uint8_t strongest_band_idx;
    unsigned long sec_strongest_value;
    uint8_t sec_strongest_band_idx;
    for(byte j = 0; j < EEG_POWER_BANDS; j++)
    {
        if (brain.eegPower[j] > strongest_value)
        {
            strongest_value = brain.eegPower[j];
            strongest_band_idx = j;
        }
        else if (brain.eegPower[j] > sec_strongest_value)
        {
            sec_strongest_value = brain.eegPower[j];
            sec_strongest_band_idx = j;
        }
    }
    byte hsv_to_rgb[3];
    double s = 1.0;
    double v = 1.0;
    // calculate S and V these from attention/meditation values (if we have them)
    if (   bitRead(brain_packets, 1)
        && bitRead(brain_packets, 2))
    {
        double s = (double)brain.attention/100;
        double v = (double)((brain.attention+brain.meditation)/2)/100;
    }
    // In case the attention/meditation measurements are too low force a minimum
    if (s < 0.3)
    {
        s = 0.3;
    }
    if (v < 0.3)
    {
        v = 0.3;
    }
    rgbconverter.hsvToRgb((eeg_band_hsv_hue[strongest_band_idx]+eeg_band_hsv_hue[sec_strongest_band_idx])/2, s, v, hsv_to_rgb);

    // Set the frame to said color
    for (byte i = 0; i < 3; i++)
    {
        eeg_frame.leds[0][i] = hsv_to_rgb[i];
        eeg_frame.leds[1][i] = hsv_to_rgb[i];
    }

    // The data comes in once a second, set wait time accordingly
    eeg_frame.wait_ms = 1000;

    if (state == STOPPED)
    {
        this->start_animation();
    }
}

void EEGAnimation::unpack_frame(const uint8_t *start_of_frame, frame_data& tgt)
{
    for (byte i = 0; i < 3; i++)
    {
        tgt.leds[0][i] = eeg_frame.leds[0][i];
        tgt.leds[1][i] = eeg_frame.leds[1][i];
    }
    tgt.wait_ms = eeg_frame.wait_ms;
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
    virtual void enable(boolean en);
};

EEGReader::EEGReader()
: Task()
{
    this->enable(true);
}

void EEGReader::enable(boolean en)
{
    if (en)
    {
        pinMode(BRAIN_POWER_PIN, OUTPUT);
        digitalWrite(BRAIN_POWER_PIN, HIGH);
        return;
    }
    eeg_anim.stop_animation();
    pinMode(BRAIN_POWER_PIN, INPUT);
    digitalWrite(BRAIN_POWER_PIN, LOW);
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
            eeg_payload[sizeof(eeg_payload)-1] = brain_packets;
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
        eeg_payload[sizeof(eeg_payload)-1] = brain_packets;
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
