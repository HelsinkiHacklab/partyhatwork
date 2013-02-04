#include <Arduino.h>

/**
 * Adjusts the PWM widht based on battery voltage
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
    analogWrite(startpin, pwmlimit(r));
    analogWrite(startpin+1, pwmlimit(g));
    analogWrite(startpin+2, pwmlimit(b));
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

