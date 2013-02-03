/**
 * Partyhatwork firmware reference implementation, you will need XMega compatible Arduino IDE, see https://github.com/HelsinkiHacklab/partyhatwork/tree/master/software/arduino-xmega
 * 
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

    // This is the XBee serial port
    Serial2.begin(57600);
    
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


void loop()
{
}
