#ifndef LoggedMeasurement_h
#define LoggedMeasurement_h

#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include "Arduino.h"

#define OLED_RESET 4
#define LOG_MAX 125

class LoggedMeasurement
{
    const unsigned long logRate = 6000;
    double offset;
    double scale;
    double calibration;

public:
    String name;
    String unit;
    double currentMeasurement;
    double measurementLog[LOG_MAX];
    unsigned long lastMeasurementTime;

    LoggedMeasurement(String pName, String pUnit, double pCalibration, double pOffset, double pScale);
    void addMeasurement(double measurement, unsigned long timeMilli);
    String toString();
};

class LogDispaly : Adafruit_SSD1306
{
    void printMeasurement(LoggedMeasurement measurement, String debug);
    void printLogFilled(LoggedMeasurement measurement, int color);
    void printLogDotted(LoggedMeasurement measurement, int color);
    void printYAxis();
    void printXAxis();

public:
    // 128x64 pixels
    LogDispaly();
    void init();
    void printOneFilled(LoggedMeasurement measurement, String debug);
    void printManyDotted(LoggedMeasurement *logs[], int mainLog, int logsSize, String debug);
};

class LogTrafficLight : Adafruit_NeoPixel
{
    double yellow;
    double red;

public:
    LogTrafficLight(double pYellow, double pRed);
    void init ();
    void showMeasurement(LoggedMeasurement measurement);
};

#endif