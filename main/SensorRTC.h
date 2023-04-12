#ifndef SENSOR_RTC_H
#define SENSOR_RTC_H
/**************************************************************************
// Date and time functions using a DS1302 RTC
 **************************************************************************/
#include <Arduino.h>
#include <RTClib.h>
//#include <DS1307RTC.h>

RTC_DS1307 sensor_rtc_setup();

#endif
