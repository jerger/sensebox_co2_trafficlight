#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "SenseBoxMCU.h"
#include "SparkFun_SCD30_Arduino_Library.h"
#include <Adafruit_NeoPixel.h>

#define OLED_RESET 4
#define ACCEL_2G 0x3
#define LOG_MAX 125

class TimedTrigger
{
  const unsigned long triggerTime = 120000;
  unsigned long lastSwitchTime;

public:
  boolean isTriggered()
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
};

class LoggedMeasurement
{
  const unsigned long logRate = 60000;
  double offset;
  double scale;
  double calibration;

public:
  String name;
  String unit;
  double currentMeasurement;
  double measurementLog[LOG_MAX];
  unsigned long lastMeasurementTime;

  LoggedMeasurement(String pName, String pUnit, double pCalibration, double pOffset, double pScale)
  {
    name = pName;
    unit = pUnit;
    calibration = pCalibration;
    offset = pOffset;
    scale = pScale;
  }

  void addMeasurement(double measurement, unsigned long timeMilli)
  {

    currentMeasurement = measurement + calibration;
    if ((lastMeasurementTime + logRate) < timeMilli)
    {
      lastMeasurementTime = timeMilli;
      for (int i = LOG_MAX - 1; i > 0; i--)
      {
        measurementLog[i] = measurementLog[i - 1];
      }
      measurementLog[0] = (currentMeasurement - offset) / scale;
    }
  }

  String toString()
  {
    return name + " (" + unit + "):" + currentMeasurement;
  }
};

class LogDispaly : Adafruit_SSD1306
{
  void printMeasurement(LoggedMeasurement measurement, String debug)
  {
    setCursor(0, 0);
    setTextSize(1);
    setTextColor(WHITE, BLACK);
    print(measurement.toString() + "/" + debug);
  }

  void printLog(LoggedMeasurement measurement)
  {
    for (byte i = 0; i < LOG_MAX; i = i + 1)
    {
      short y = 61 - int(measurement.measurementLog[i] * 48);
      drawLine(i + 3, 61, i + 3, y, 2);
    }
  }

  void printYAxis()
  {
    drawLine(2, 14, 2, 60, 1);
    // y-ticks: 4 * 12 = 48
    for (byte i = 0; i < 4; i++)
    {
      short y = 14 + (i * 12);
      drawLine(0, y, 1, y, 1);
    }
  }

  void printXAxis()
  {
    drawLine(0, 62, 128, 62, 1);
    // x-ticks: 6 * 21 = 126
    for (byte i = 0; i < 6; i++)
    {
      int x = 23 + (i * 21);
      drawLine(x, 63, x, 64, 1);
    }
  }

public:
  // 128x64 pixels
  LogDispaly() : Adafruit_SSD1306(OLED_RESET) {}

  void init()
  {
    delay(3000);
    begin(SSD1306_SWITCHCAPVCC, 0x3D);
    display();
    delay(1000);
    clearDisplay();
  }

  void printOnDisplay(LoggedMeasurement measurement, String debug)
  {
    clearDisplay();
    printMeasurement(measurement, debug);
    printYAxis();
    printXAxis();
    printLog(measurement);
    display();
  }
};

int mode;

// init globals
LoggedMeasurement co2 = LoggedMeasurement("CO2", "ppm", 0.0, 500.0, 2000.0);
LoggedMeasurement temp = LoggedMeasurement("Temp", "*C", -5.5, 15.0, 20.0);
LoggedMeasurement humidity = LoggedMeasurement("Hum", "%", 0.0, 0.0, 100.0);
LoggedMeasurement *logs[] = {&co2, &temp, &humidity};

LogDispaly display;
SCD30 airSensor;
TimedTrigger aTrigger;
Adafruit_NeoPixel rgb_led_1 = Adafruit_NeoPixel(3, 1, NEO_GRB + NEO_KHZ800);

// output of co2 to attached led
void co2_to_led(LoggedMeasurement measurement)
{
  if (measurement.currentMeasurement < 1000.0)
  {
    rgb_led_1.setPixelColor(0, rgb_led_1.Color(0, 255, 0));
    rgb_led_1.show();
  }
  else if (measurement.currentMeasurement >= 1000.0 && measurement.currentMeasurement < 1500.0)
  {
    rgb_led_1.setPixelColor(0, rgb_led_1.Color(127, 127, 0));
    rgb_led_1.show();
  }
  else if (measurement.currentMeasurement >= 1500.0)
  {
    rgb_led_1.setPixelColor(0, rgb_led_1.Color(255, 0, 0));
    rgb_led_1.show();
  }
}

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

  rgb_led_1.begin();
  rgb_led_1.setBrightness(100);
}

void loop()
{
  unsigned long t = millis();
  co2.addMeasurement(float(airSensor.getCO2()), t);
  temp.addMeasurement(airSensor.getTemperature(), t);
  humidity.addMeasurement(airSensor.getHumidity(), t);

  if (aTrigger.isTriggered())
  {
    mode = (mode + 1) % 3;
  }

  co2_to_led(co2);
  display.printOnDisplay(*logs[mode], String(t / 60000.0));
  delay(30000);;
}
