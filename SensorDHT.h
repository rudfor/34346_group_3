#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H
#include <Arduino.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11

void DHTbegin();
float readHumidity();
float readTemperature();

#endif