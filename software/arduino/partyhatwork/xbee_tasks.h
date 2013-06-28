#ifndef XBEE_TASKS_H
#ifndef XBEE_RESET_PIN
#error "XBEE_RESET_PIN not defined"
#endif
#ifndef XBEE_SERIAL
#error "XBEE_SERIAL not defined"
#endif
#define XBEE_TASKS_H
#include <Arduino.h>
#include <Task.h>
#include <XBee.h>

// Reminder, XBee uses by defaul the first Serial object ("Serial"), but we override that in the setup() function in partyhatwork.ino
XBee xbee = XBee();
extern XBee xbee;
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

/**
 * Resets the XBee module using the HW-reset pin
 */
void reset_xbee()
{
    digitalWrite(XBEE_RESET_PIN, LOW);
    delay(20);
    digitalWrite(XBEE_RESET_PIN, HIGH);
}

typedef void (*XBeeReadvoidFuncPtr)(ZBRxResponse);


#define XBEE_READ_YIELD_TICKS 4
class XBeeRead : public Task
{
public:
//    XBeeRead(XBeeReadvoidFuncPtr _callback);
    XBeeRead();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
    XBeeReadvoidFuncPtr callback;
private:
    uint8_t yield_counter; // Incremented on each canRun call, used to yield to other tasks.
};

/*
XBeeRead::XBeeRead(XBeeReadvoidFuncPtr _callback)
: Task(),
  callback(_callback)
{
}
*/
XBeeRead::XBeeRead()
: Task()
{
}

// We can't just return true since then no other task could ever run (since we have the priority)
bool XBeeRead::canRun(uint32_t now)
{
    return (boolean)XBEE_SERIAL.available();
    /*
    yield_counter++;
    return ((yield_counter % XBEE_READ_YIELD_TICKS) == 1);
    */
}

void XBeeRead::run(uint32_t now)
{
    xbee.readPacket();
    if (xbee.getResponse().isAvailable())
    {
        // got something
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
        {
            // got a zb rx packet
            xbee.getResponse().getZBRxResponse(rx);
            this->callback(rx);
        }
    }
}

XBeeRead xbeereader;
extern XBeeRead xbeereader;


#endif
