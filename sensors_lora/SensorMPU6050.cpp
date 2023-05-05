#include "SensorMPU6050.h"

Adafruit_MPU6050 sensor_mpu_6050_setup(){
    Adafruit_MPU6050 mpu;
    if (!mpu.begin(0x69)) {
        Serial.println("Sensor init failed");
        while (1)
            yield();
    }
    Serial.println("Found a MPU-6050 sensor");

    return mpu;
}
