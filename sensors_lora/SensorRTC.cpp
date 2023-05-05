#include "SensorRTC.h"

// Init rtc object
//   DS1302 rtc;
//   DS1302 rtc(ce_pin, sck_pin, io_pin);
//
// ce_pin  (RST): default 4
// sck_pin (CLK): default 5
// io_pin  (DAT): default 6
//DS1302 rtc;

RTC_DS1307 sensor_rtc_setup() {
  RTC_DS1307 rtc;
  /*only set the date+time one time* / rtc.set(0, 0, 8, 24, 12, 2014); /*08:00:00 24.12.2014 //sec, min, hour, day, month, year*/
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  //  rtc.begin();
  //rtc.start();
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));
    rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  return rtc;
}
