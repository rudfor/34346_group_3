#ifndef LORA_DEV_SETUP_H
#define LORA_DEV_SETUP_H

#include <SoftwareSerial.h>
#include <Arduino.h>

extern SoftwareSerial loraSerial;

void lorawan_setup();

#endif
