#include "SensorDHT.h"

DHT dht(DHTPIN, DHTTYPE);

void DHTbegin(){
    dht.begin();
}

float readHumidity(){
    return dht.readHumidity();
}

float readTemperature(){
    return dht.readTemperature();
}