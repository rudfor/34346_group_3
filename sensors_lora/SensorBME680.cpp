#include "SensorBME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 sensor_bme_680_setup(){
  Adafruit_BME680 bme; // I2C
  if (!bme.begin(0x76)) {
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
  while (1);
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  return bme;
}
