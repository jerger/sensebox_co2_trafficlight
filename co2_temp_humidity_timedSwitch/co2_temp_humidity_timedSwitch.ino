#include <SPI.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"
#include "SwitchAndTrigger.h"
#include "LoggedMeasurement.h"

// init globals
LoggedMeasurement co2 = LoggedMeasurement("CO2", "ppm", 0.0, 500.0, 2000.0);
LoggedMeasurement temp = LoggedMeasurement("Temp", "*C", -5.5, 15.0, 20.0);
LoggedMeasurement humidity = LoggedMeasurement("Hum", "%", 0.0, 0.0, 100.0);
LoggedMeasurement *logs[] = {&co2, &temp, &humidity};

LogDispaly display;
SCD30 airSensor;
TimedTrigger aTrigger;
LogTrafficLight led = LogTrafficLight(1000.0, 1500.0);

unsigned long lastMeasurementTime;
int mode;

void setup()
{
  senseBoxIO.powerI2C(true);
  display.init();

  Wire.begin();
  if (airSensor.begin() == false)
  {
    while (1)
      ;
  }

  led.init();
}

void loop()
{
  unsigned long t = millis();
  String hour = String(t / 3600000.0);

  if (t > lastMeasurementTime + 30000)
  {
    lastMeasurementTime = t;
    co2.addMeasurement(float(airSensor.getCO2()), t);
    temp.addMeasurement(airSensor.getTemperature(), t);
    humidity.addMeasurement(airSensor.getHumidity(), t);
  }

  if (aTrigger.isTriggered())
  {
    mode = (mode + 1) % 3;
  }

  led.showMeasurement(co2);
  display.printOneFilled(*logs[mode], hour);
}
