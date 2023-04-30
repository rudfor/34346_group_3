#ifndef DEV_SDCARD_H
#define DEV_SDCARD_H
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

void setupSD();
void logSD(String msg_title, String msg_line1, String msg_line2, String msg_line3, String msg_line4, String msg_line5, String msg_line6, String msg_line7);

#endif
