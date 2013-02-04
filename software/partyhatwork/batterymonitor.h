#include <Arduino.h>
#include <Task.h>

class BatteryMonitor : public TimedTask
{
public:
    // Create a new blinker for the specified pin and rate.
    BatteryMonitor(uint8_t _pin);
    virtual void run(uint32_t now);
    int bat_mv;
    int adc_value;
private:
    uint8_t pin;      // ADC pin
};

BatteryMonitor::BatteryMonitor(uint8_t _pin)
: TimedTask(millis()),
  pin(_pin)
{
    pinMode(pin, INPUT);     // Set pin for output.
}

void BatteryMonitor::run(uint32_t now)
{
    // Start by setting the next runtime
    incRunTime(15000);
    adc_value = analogRead(pin);
    // TODO: Map the value to mV
    // TODO: keep a running average for less noisy voltage
}

