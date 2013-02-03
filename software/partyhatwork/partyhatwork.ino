/**
 * Partyhatwork firmware reference implementation, you will need XMega compatible Arduino IDE, see https://github.com/HelsinkiHacklab/partyhatwork/tree/master/software/arduino-xmega
 * 
 * Ge the XBee library from http://code.google.com/p/xbee-arduino/
 * 
   Arduino Pin map

            USB
    0   PE0     GND
    1   PE1     GND
    2   PE2     PD5   29
    3   PE3     PD4   28
    4   PA0     PD3   27
    5   PA1     PD2   26
    6   PA2     PD1   25
    7   PA3     PD0   24
    8   PA4     PC7   23
    9   PA5     PC6   22
   10   PA6     PC5   21
   11   PA7     PC4   20
   12   PB0     PC3   19
   13   PB1     PC2   18
   14   PB2     PC1   17
   15   PB3     PC0   16


 * 
 */

#define BAT_SENSE AN9
#define HIGH_CURRENT_CHG 12
#define XBEE_RESET 28

#include <XBee.h>
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup()
{
    // Initialize the 6 PWM with MOSFETs
    for (byte i=17; i < 23; i++)
    {
        pinMode(i, OUTPUT);
    }
    // XBee reset pin, drive low to reset
    pinMode(XBEE_RESET, OUTPUT);
    reset_xbee();

    // Initialize the XBee wrapper
    Serial2.begin(57600);
    xbee.begin(Serial2);
    
}

/**
 * Resets the XBee module using the HW-reset pin
 */
void reset_xbee()
{
    digitalWrite(XBEE_RESET, LOW);
    delay(20);
    digitalWrite(XBEE_RESET, HIGH);
}

/**
 * Adjusts the PWM widht based on battery voltage
 *
 * TODO: Implement some actual adjustment curve
 */
uint8_t pwmlimit(uint8_t pwm)
{
    // This removes a quater from the value
    return pwm - (pwm << 2);
}

/**
 * Generic setter for 3 consequtve PWM pins
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
    setRGB(17, r,g,b);
}

/**
 * Set the second set of built-in drivers
 */
inline void setRGB1(uint8_t r, uint8_t g, uint8_t b)
{
    setRGB(20, r,g,b);
}


void loop()
{
    xbee.readPacket();
    if (xbee.getResponse().isAvailable())
    {
        // got something
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
        {
            // got a zb rx packet
            xbee.getResponse().getZBRxResponse(rx);
        }
        uint8_t led = rx.getData(0);
        if (led == 1)
        {
            setRGB1(rx.getData(1),rx.getData(2),rx.getData(3));
        }
        else
        {
            setRGB0(rx.getData(1),rx.getData(2),rx.getData(3));
        }
    }

}
