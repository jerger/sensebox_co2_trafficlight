#include "SwitchAndTrigger.h"

void DebouncedGyroSwitch::init()
{
    beginGyro();
}

boolean DebouncedGyroSwitch::isTriggered()
{
    boolean result = false;
    int x, y, z;
    unsigned long t = millis();
    if (t > cooldown + lastSwitchTime)
    {
        lastSwitchTime = t;
        getRotation(&x, &y, &z);
        result = z < -4 ||  z > 4;
    }
    return result;
}

void DebouncedAccelerometerSwitch::init()
{
    beginAcc(ACCEL_2G);
    normal = getAccelerationZ();
}

boolean DebouncedAccelerometerSwitch::isTriggered()
{
    boolean result = false;
    unsigned long t = millis();
    if (t > cooldown + lastSwitchTime)
    {
        lastSwitchTime = t;
        result = getAccelerationZ() > normal + 0.2;
    }
    return result;
}

boolean TimedTrigger::isTriggered()
{
    boolean result = false;
    unsigned long t = millis();
    if (t > triggerTime + lastSwitchTime)
    {
        lastSwitchTime = t;
        result = true;
    }
    return result;
}