#include "LoggedMeasurement.h"

LoggedMeasurement::LoggedMeasurement(String pName, String pUnit, double pCalibration, double pOffset, double pScale)
{
  name = pName;
  unit = pUnit;
  calibration = pCalibration;
  offset = pOffset;
  scale = pScale;
}

void LoggedMeasurement::addMeasurement(double measurement, unsigned long timeMilli)
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

String LoggedMeasurement::toString()
{
  return name + " (" + unit + "):" + currentMeasurement;
}

LogDispaly::LogDispaly() : Adafruit_SSD1306(OLED_RESET) {}

void LogDispaly::init()
{
  delay(2000);
  begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display();
  delay(500);
  clearDisplay();
}

void LogDispaly::printMeasurement(LoggedMeasurement measurement, String debug)
{
  setCursor(0, 0);
  setTextSize(1);
  setTextColor(WHITE, BLACK);
  print(measurement.toString() + "/" + debug);
}

void LogDispaly::printLogFilled(LoggedMeasurement measurement, int color)
{
  for (byte i = 0; i < LOG_MAX; i++)
  {
    short y = 61 - int(measurement.measurementLog[i] * 48);
    drawLine(i + 3, 61, i + 3, y, color);
  }
}

void LogDispaly::printLogDotted(LoggedMeasurement measurement, int color)
{
  short y0 = 0;
  for (byte i = 0; i < LOG_MAX; i++)
  {
    short y1 = 61 - int(measurement.measurementLog[i] * 48);
    drawLine(i+2, y0, i + 3, y1, color);
    y0 = y1;
  }
}

void LogDispaly::printYAxis()
{
  drawLine(2, 14, 2, 60, 1);
  // y-ticks: 4 * 12 = 48
  for (byte i = 0; i < 4; i++)
  {
    short y = 14 + (i * 12);
    drawLine(0, y, 1, y, 1);
  }
}

void LogDispaly::printXAxis()
{
  drawLine(0, 62, 128, 62, 1);
  // x-ticks: 6 * 21 = 126
  for (byte i = 0; i < 6; i++)
  {
    int x = 2 + (i * 21);
    drawLine(x, 63, x, 64, 1);
  }
}

void LogDispaly::printOneFilled(LoggedMeasurement measurement, String debug)
{
  clearDisplay();
  printMeasurement(measurement, debug);
  printYAxis();
  printXAxis();
  printLogFilled(measurement, 2);
  display();
}

void LogDispaly::printManyDotted(LoggedMeasurement *logs[], int mainLog, int logsSize, String debug)
{
  clearDisplay();
  printMeasurement(*logs[mainLog], debug);
  printYAxis();
  printXAxis();
  printLogDotted(*logs[mainLog], 2);
  for (int i = 1; i < logsSize; i++)
  {
    printLogDotted(*logs[(mainLog + i) % logsSize], 2 + i*2);
  }
  display();
}

LogTrafficLight::LogTrafficLight(double pYellow, double pRed) : Adafruit_NeoPixel(3, 1, NEO_GRB + NEO_KHZ800)
{
  yellow = pYellow;
  red = pRed;
}

void LogTrafficLight::showMeasurement(LoggedMeasurement measurement)
{
  if (measurement.currentMeasurement < yellow)
  {
    setPixelColor(0, Color(0, 255, 0));
    show();
  }
  else if (measurement.currentMeasurement >= yellow && measurement.currentMeasurement < red)
  {
    setPixelColor(0, Color(127, 127, 0));
    show();
  }
  else if (measurement.currentMeasurement >= red)
  {
    setPixelColor(0, Color(255, 0, 0));
    show();
  }
}

void LogTrafficLight::init()
{
  begin();
  setBrightness(100);
}
