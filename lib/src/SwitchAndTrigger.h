#ifndef SwitchAndTrigger_h
#define SwitchAndTrigger_h

#include "Arduino.h"
#include "SenseBoxMCU.h"

#define ACCEL_2G 0x3

class DebouncedGyroSwitch : BMX055
{
    const unsigned long cooldown = 10000;
    double normal;
    unsigned long lastSwitchTime;

public:
    void init();
    boolean isTriggered();
};

class DebouncedAccelerometerSwitch : BMX055
{
    const unsigned long cooldown = 1000;
    double normal;
    unsigned long lastSwitchTime;

public:
    void init();
    boolean isTriggered();
};

class TimedTrigger
{
    const unsigned long triggerTime = 120000;
    unsigned long lastSwitchTime;

public:
    boolean isTriggered();
};

#endif
