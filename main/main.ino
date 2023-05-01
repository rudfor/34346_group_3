// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include <ESP8266WiFi.h>
#include <RTClib.h>
#include <string.h>
//#include "src/iotc/common/string_buffer.h"
//#include "src/iotc/iotc.h"
// can be replaced with your own copy #include "config_USER.h in gitignore"
#include "config.h"
#include "Oled.h"
#include "SensorDHT.h"
#include "SensorRTC.h"
#include "SensorBME680.h"
#include "SensorMPU6050.h"
#include "SDCard.h"
#include "lora_dev_setup.h"
#include "protocol.h"

#define SENSORPIN A0

int lastTickDisplay;
int lastTickDHT;
int lastTickRTC;
int lastTickBME;
int lastTickPhotoResistor;
int lastTickMPU;
int lastTickMes;
int lastTickSD;

bool alternate = false;  // Alternate display between 2 screens of values
bool sensorRead = false;

int RxDelay1 = 3000;
int RxDelay2 = 7000;

// Timers for frequency / itteration / Duration between: tasks every X milis run the task
int timerDisplay = 5000;
int timerDHT = 5000;
int timerRTC = 5000;
int timerBME = 5000;
int timerMPU = 5000;
int timerPR = 5000;
int timerSDC = 5000;

// Sensor thresholds
int thresBME_temp_min = 15;
int thresBME_temp_max = 25;
int thresBME_hum_min = 10;
int thresBME_hum_max = 45;
int thresPR_min = 0;
int thresPR_max = 500;

// alert booleans
bool alertBME_temp = false;
bool alertBME_hum = false;
bool alertPR = false;

bool sentAlertBME_temp = false;
bool sentAlertBME_hum = false;
bool sentAlertPR = false;

// Is the Sensor Connected
bool DHT = false;  // Removed Duplicate Functionality to the BME680
bool RTC = true;   // Real Time Clock
bool BME = true;   //
bool MPU = false;  // MPU - Multi Precision Unit Disabled due to integration error
bool PR = true;    // Photo Resistor
bool SDC = true;   // SDCard

bool receiveMode = false;

// Is the Sensor Active
bool DHT_active = false;
bool RTC_active = false;
bool BME_active = false;
bool MPU_active = false;
bool PR_active = false;
bool SDC_format = false;
bool SDC_active = false;

// sensor values
float dhtH;
float dhtT;
float bmeH;
float bmeT;
int PhotoResValue;

// Strings for the Messages
char msg_humidity[22];
char msg_temp[22];
char msg_hum_temp[22];
char msg_serial[22];
char msg_azure[22];
char msg_rtc[22];
char msg_bme1[22];
char msg_bme2[22];
char msg_bme3[22];
char msg_photores[22];
char msg_mpu_accx[22];
char msg_mpu_accy[22];
char msg_mpu_accz[22];
char msg_mpu_gyrx[22];
char msg_mpu_gyry[22];
char msg_mpu_gyrz[22];

String response;
String message;
String data;

RTC_DS1307 rtc;
Adafruit_BME680 bme;   // I2C
Adafruit_MPU6050 mpu;  // I2C

char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

String appEUI = "017f";
String devEUI = "ed96";

void setup() {
  Wire.begin(2, 0);  // SDA, SCL
  // turn on Serial
  Serial.begin(9600);
  // Setup OLED
  oled_setup();
  setupSD();

  if (RTC) { rtc = sensor_rtc_setup(); }
  if (BME) { bme = sensor_bme_680_setup(); }
  if (MPU) { mpu = sensor_mpu_6050_setup(); }

  if (DHT) { DHTbegin(); }
  lorawan_setup();
}

void loop() {

  /***
 *      ___   _            _                ___   _          _  
 *     |   \ (_) ___ _ __ | | __ _  _  _   / _ \ | | ___  __| | 
 *     | |) || |(_-<| '_ \| |/ _` || || | | (_) || |/ -_)/ _` | 
 *     |___/ |_|/__/| .__/|_|\__,_| \_, |  \___/ |_|\___|\__,_| 
 *                  |_|             |__/                        
 */
  /* Display */
  unsigned long ms_disp = millis();
  if (ms_disp - lastTickDisplay > timerDisplay && alternate) {
    lastTickDisplay = ms_disp;
    alternate = !alternate;
    screen_display("group3: transport", msg_rtc, msg_bme1, msg_bme2, msg_bme3, msg_hum_temp, msg_photores, "");
  }
  if (ms_disp - lastTickDisplay > timerDisplay && !alternate) {
    lastTickDisplay = ms_disp;
    alternate = !alternate;
    screen_display("group3: transport", msg_rtc, msg_mpu_accx, msg_mpu_accy, msg_mpu_accz, msg_mpu_gyrx, msg_mpu_gyry, msg_mpu_gyrz);
  }

  /***
 *      ___  ___    ___                _  __      __     _  _         
 *     / __||   \  / __| __ _  _ _  __| | \ \    / /_ _ (_)| |_  ___  
 *     \__ \| |) || (__ / _` || '_|/ _` |  \ \/\/ /| '_|| ||  _|/ -_) 
 *     |___/|___/  \___|\__,_||_|  \__,_|   \_/\_/ |_|  |_| \__|\___| 
 *                                                                    
 */
  /* SDCard Write */
  unsigned long ms_sdc = millis();
  if (ms_sdc - lastTickSD > timerSDC) {
    lastTickSD = ms_sdc;
    alternate = !alternate;
    logSD("group3: transport", msg_rtc, msg_bme1, msg_bme2, msg_bme3, msg_hum_temp, msg_photores, "");
    logSD("group3: transport", msg_rtc, msg_mpu_accx, msg_mpu_accy, msg_mpu_accz, msg_mpu_gyrx, msg_mpu_gyry, msg_mpu_gyrz);
  }

  /*
*  _        ___        __  __                      _           
* | |   ___| _ \__ _  |  \/  |___ ______ __ _ __ _(_)_ _  __ _ 
* | |__/ _ \   / _` | | |\/| / -_|_-<_-</ _` / _` | | ' \/ _` |
* |____\___/_|_\__,_| |_|  |_\___/__/__/\__,_\__, |_|_||_\__, |
*                                            |___/       |___/ 
*/

  unsigned long ms_mes = millis();

  alertBME_temp = ((bmeT > thresBME_temp_max) || (bmeT < thresBME_temp_min)) & !sentAlertBME_temp;
  alertBME_hum = ((bmeH > thresBME_hum_max) || (bmeH < thresBME_hum_min)) & !sentAlertBME_hum;
  alertPR = ((PhotoResValue > thresPR_max) || (PhotoResValue < thresPR_min)) & !sentAlertPR;

  if (alertBME_temp || alertBME_hum || alertPR) {  // TO-DO change to if there is an alert
    lastTickMes = ms_disp;

    // send a message
    if (alertBME_temp) {
      sentAlertBME_temp = true;
      loraSerial.print("radio tx ");
      loraSerial.print(appEUI);
      loraSerial.print(devEUI);
      message = encodeAlertMessage(TEMPERATURE, true, true, bmeT);
      loraSerial.println(message);
      Serial.println(message);
      // we will get two responses from the rn2483
      // this one should be "ok", and it means parameter is valid
      response = loraSerial.readStringUntil('\n');
      Serial.println(response);

      // this one should be "radio_tx_ok", and it means the transmission was successful
      response = loraSerial.readStringUntil('\n');
      Serial.println(response);
    }
    if (alertBME_hum) {
      sentAlertBME_hum = true;
      loraSerial.print("radio tx ");
      loraSerial.print(appEUI);
      loraSerial.print(devEUI);
      message = encodeAlertMessage(HUMIDITY, true, true, bmeH);
      loraSerial.println(message);
      Serial.println(message);
      // we will get two responses from the rn2483
      // this one should be "ok", and it means parameter is valid
      response = loraSerial.readStringUntil('\n');
      Serial.println(response);

      // this one should be "radio_tx_ok", and it means the transmission was successful
      response = loraSerial.readStringUntil('\n');
      Serial.println(response);
    }
    if (alertPR) {
      sentAlertPR = true;
      loraSerial.print("radio tx ");
      loraSerial.print(appEUI);
      loraSerial.print(devEUI);
      message = encodeAlertMessage(PHOTODIODE, true, true, PhotoResValue);
      loraSerial.println(message);
      Serial.println(message);
      // we will get two responses from the rn2483
      // this one should be "ok", and it means parameter is valid
      response = loraSerial.readStringUntil('\n');
      Serial.println(response);

      // this one should be "radio_tx_ok", and it means the transmission was successful
      response = loraSerial.readStringUntil('\n');
      Serial.println(response);
    }



    receiveMode = true;
  }

  //if (ms_disp - lastTickMes > RxDelay1) {
  //  // receive first time
  //  loraSerial.println("radio rx 1");
  //    delay(20);
  //
  //    if (response.indexOf("ok") == 0)
  //    {
  //      // if we are in receive mode, we will wait until we get a message
  //      response = String("");
  //      while (response == "")
  //      {
  //        response = loraSerial.readStringUntil('\n');
  //      }
  //
  //      // the second message can either be "radio_rx" or "radio_err"
  //      if (response.indexOf("radio_rx") == 0)
  //      {
  //        data = response.substring(10, 34); // we get the message
  //        Serial.println(data); // TO-DO do something with the message
  //      }
  //    }
  //}
  //
  //if (ms_disp - lastTickMes > RxDelay2) {
  //  // receive second time
  //  loraSerial.println("radio rx 1");
  //    delay(20);
  //
  //    if (response.indexOf("ok") == 0)
  //    {
  //      // if we are in receive mode, we will wait until we get a message
  //      response = String("");
  //      while (response == "")
  //      {
  //        response = loraSerial.readStringUntil('\n');
  //      }
  //
  //      // the second message can either be "radio_rx" or "radio_err"
  //      if (response.indexOf("radio_rx") == 0)
  //      {
  //        data = response.substring(10, 34); // we get the message
  //        Serial.println(data); // TO-DO do something with the message
  //      }
  //    }
  //
  //  receiveMode = false;
  //}

  /***
 *      ___             _   _____  _                ___  _           _    
 *     | _ \ ___  __ _ | | |_   _|(_) _ __   ___   / __|| | ___  __ | |__ 
 *     |   // -_)/ _` || |   | |  | || '  \ / -_) | (__ | |/ _ \/ _|| / / 
 *     |_|_\\___|\__,_||_|   |_|  |_||_|_|_|\___|  \___||_|\___/\__||_\_\ 
 *                                                                        
 */
  /* RTC */
  unsigned long ms_rtc = millis();
  if (RTC && ms_rtc - lastTickRTC > timerRTC) {
    lastTickRTC = ms_rtc;
    DateTime now = rtc.now();
    snprintf(msg_rtc, 22, "%2d/%2d/%4d %2d:%2d:%2d", now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
    //snprintf(msg_rtc, 22, "%ld/%ld/%ld %ld:%ld", now.day(),now.month(),now.year(), now.hour(),now.minute());
    Serial.print("Date & Time: ");
    Serial.println(msg_rtc);
  }

  /***
 *      ___                            ___   _  _  _____ 
 *     / __| ___  _ _   ___ ___  _ _  |   \ | || ||_   _|
 *     \__ \/ -_)| ' \ (_-</ _ \| '_| | |) || __ |  | |  
 *     |___/\___||_||_|/__/\___/|_|   |___/ |_||_|  |_|  
 *                                                       
 */
  /* Sensor DHT 11 */
  unsigned long ms_dht = millis();
  if (ms_dht - lastTickDHT > timerDHT) {
    lastTickDHT = ms_disp;
    dhtH = readHumidity();
    dhtT = readTemperature();
    snprintf(msg_humidity, 22, "h: %3.1f", dhtH);
    snprintf(msg_temp, 22, "t: %3.1f C", dhtT);
    snprintf(msg_hum_temp, 22, "DHT| h:%3.1f%% t:%3.1fC ", dhtH, dhtT);
    Serial.println(String(msg_hum_temp));
    //Serial.print(F("Humidity: ")); Serial.print(String(dhtH)); Serial.print(F(" [%]\t"));
    //Serial.print(F("Temp: ")); Serial.print(dhtT); Serial.println(F(" [C]"));
  }

  /***
 *      ___  _          _         ___           _      _             
 *     | _ \| |_   ___ | |_  ___ | _ \ ___  ___(_) ___| |_  ___  _ _ 
 *     |  _/| ' \ / _ \|  _|/ _ \|   // -_)(_-<| |(_-<|  _|/ _ \| '_|
 *     |_|  |_||_|\___/ \__|\___/|_|_\\___|/__/|_|/__/ \__|\___/|_|  
 *                                                                   
 */
  /* Sensor Analog Input Photo Resistor */
  unsigned long ms_pres = millis();
  if (ms_pres - lastTickPhotoResistor > timerPR) {
    lastTickPhotoResistor = ms_disp;
    PhotoResValue = analogRead(SENSORPIN);
    sentAlertPR;
    snprintf(msg_photores, 22, "P_res: %5d", PhotoResValue);
    Serial.println(String(msg_photores));
  }
  /*int sensorPin A0;
char msg_photores[22];
int lastTickPhotoResistor;*/


  /***
 *      ___                            ___  __  __  ___    __  ___   __  
 *     / __| ___  _ _   ___ ___  _ _  | _ )|  \/  || __|  / / ( _ ) /  \ 
 *     \__ \/ -_)| ' \ (_-</ _ \| '_| | _ \| |\/| || _|  / _ \/ _ \| () |
 *     |___/\___||_||_|/__/\___/|_|   |___/|_|  |_||___| \___/\___/ \__/ 
 *                                                                       
 */
  /* Sensor BME680 */
  unsigned long ms_bme = millis();
  if (ms_bme - lastTickBME > timerBME) {
    lastTickBME = ms_disp;
    //float dhtHumidity = dht.readHumidity();
    if (!bme.performReading()) {
      Serial.println("Failed to perform reading :(");
    } else {
      //Serial.print("Pressure = "); Serial.print(bme.pressure / 100.0); Serial.println(" hPa");
      //display.print("Pressure: "); display.print(bme.pressure / 100); display.println(" hPa");
      //Serial.print("Gas = "); Serial.print(bme.gas_resistance / 1000.0); Serial.println(" KOhms");
      //display.print("Gas: "); display.print(bme.gas_resistance / 1000.0); display.println(" KOhms");
      bmeH = bme.humidity;
      bmeT = bme.temperature;
      sentAlertBME_temp = false;
      sentAlertBME_hum = false;
      snprintf(msg_bme1, 22, "BME| h:%3.1f%% t:%3.1fC", bmeH, bmeT);
      Serial.println(String(msg_bme1));
      //Serial.print("Temperature = "); Serial.print(bme.temperature); Serial.println(" *C");
      //display.print("Temperature: "); display.print(bme.temperature); display.println(" *C");
      //Serial.print("Humidity = "); Serial.print(bme.humidity); Serial.println(" %");
      //display.print("Humidity: "); display.print(bme.humidity); display.println(" %");
      snprintf(msg_bme2, 22, "BME| P:   %d  hPa", (bme.pressure / 100));
      Serial.println(String(msg_bme2));
      snprintf(msg_bme3, 22, "BME| Gas:%3.2f KOhms", (bme.gas_resistance / 1000.0));
      Serial.println(String(msg_bme3));
    }
  }

  /***
 *      ___                            __  __  ___  _   _  ___   __    __   __   
 *     / __| ___  _ _   ___ ___  _ _  |  \/  || _ \| | | || __| /  \  / /  /  \  
 *     \__ \/ -_)| ' \ (_-</ _ \| '_| | |\/| ||  _/| |_| ||__ \| () |/ _ \| () | 
 *     |___/\___||_||_|/__/\___/|_|   |_|  |_||_|   \___/ |___/ \__/ \___/ \__/  
 *                                                                               
 */
  /* Sensor MPU5060 */
  unsigned long ms_mpu = millis();
  if (MPU and ms_mpu - lastTickMPU > timerMPU) {
    lastTickMPU = ms_disp;
    Serial.println(F("MPU debug1"));
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    Serial.println(F("MPU debug2"));
    if (!sensorRead) {
      Serial.println(F("MPU debug3"));
      Serial.println("Failed to perform reading :(");
    } else {
      Serial.println(F("MPU debug4"));
      snprintf(msg_mpu_accx, 22, "MPU|A x:%2.1f ", a.acceleration.x);
      Serial.println(String(msg_mpu_accx));
      snprintf(msg_mpu_accy, 22, "MPU|A y:%2.1f ", a.acceleration.y);
      Serial.println(String(msg_mpu_accy));
      snprintf(msg_mpu_accz, 22, "MPU|A z:%2.1f ", a.acceleration.z);
      Serial.println(String(msg_mpu_accz));
      snprintf(msg_mpu_gyrx, 22, "MPU|G x:%2.1f", g.gyro.x);
      Serial.println(String(msg_mpu_gyrx));
      snprintf(msg_mpu_gyry, 22, "MPU|G y:%2.1f", g.gyro.y);
      Serial.println(String(msg_mpu_gyry));
      snprintf(msg_mpu_gyrz, 22, "MPU|G z:%2.1f", g.gyro.z);
      Serial.println(String(msg_mpu_gyrz));
    }
  }
}
