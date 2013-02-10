#ifndef RGB_H
#define RGB_H
#include <Arduino.h>
#include "gammaramp.h"

/**
 * Adjusts the PWM width based on battery voltage
 *
 * TODO: Implement some actual adjustment curve
 */
uint8_t pwmlimit(uint8_t pwm)
{
    //int foo = batterymonitor.bat_mv;
    return pwm - (pwm >> 2);
}

/**
 * Generic setter for 3 consequtve PWM pins
 *
 * TODO: Apply gamma correction
 */
void setRGB(uint8_t startpin, uint8_t r, uint8_t g, uint8_t b)
{
    byte rgamma = gammaRamp256(r);
    byte bgamma = gammaRamp256(b);
    byte ggamma = gammaRamp256(g);
    analogWrite(startpin, pwmlimit(rgamma));
    analogWrite(startpin+1, pwmlimit(ggamma));
    analogWrite(startpin+2, pwmlimit(bgamma));
}

/**
 * Set the first set of built-in drivers
 */
inline void setRGB0(uint8_t r, uint8_t g, uint8_t b)
{
    setRGB(16, r,g,b);
}

/**
 * Set the second set of built-in drivers
 */
inline void setRGB1(uint8_t r, uint8_t g, uint8_t b)
{
    setRGB(19, r,g,b);
}


#endif
