// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include <ESP8266WiFi.h>
#include <RTClib.h>
#include "src/iotc/common/string_buffer.h"
#include "src/iotc/iotc.h"
// can be replaced with your own copy #include "config_USER.h in gitignore"
#include "config_ruft.h"
#include "Oled.h"
#include "SensorDHT.h"
#include "SensorRTC.h"
#include "SensorBME680.h"

#define SENSORPIN A0

int delayVal;
int lastTickDisplay;
int lastTickDHT;
int lastTickRTC;
int lastTickBME;
int lastTickPhotoResistor;

char msg_humidity[75];
char msg_temp[75];
char msg_hum_temp[22];
char msg_serial[75];
char msg_azure[75];
char msg_rtc[75];
char msg_bme1[22];
char msg_bme2[22];
char msg_bme3[22];
char msg_photores[22];

RTC_DS1307 rtc;
Adafruit_BME680 bme; // I2C

char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo);
#include "src/connection.h"

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo) {
  // ConnectionStatus
  if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
    LOG_VERBOSE("Is connected ? %s (%d)",
                callbackInfo->statusCode == IOTC_CONNECTION_OK ? "YES" : "NO",
                callbackInfo->statusCode);
    isConnected = callbackInfo->statusCode == IOTC_CONNECTION_OK;
    return;
  }

  // payload buffer doesn't have a null ending.
  // add null ending in another buffer before print
  AzureIOT::StringBuffer buffer;
  if (callbackInfo->payloadLength > 0) {
    buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
  }

  LOG_VERBOSE("- [%s] event was received. Payload => %s\n",
              callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

  /* TODO
  Parser code to be added here
  Commands:
    sensor <on|off>
    delay <SET int>
    read <sensor>
    write <log>
    poll // is alive?
  */
  if (strcmp(callbackInfo->eventName, "Command") == 0) {
    LOG_VERBOSE("- Command name was => %s\r\n", callbackInfo->tag);
    String myString = String(callbackInfo->tag);
    if(myString.indexOf("DELAY")>=0) {
      LOG_VERBOSE("- Command name was => %s\r\n", (char*)String("Delay Command").c_str());
      // Converting const char pointer callbackInfo->tag to a String
      String myString = String(callbackInfo->tag);
      // Testing if text string val (e.g 3000) is present in myString
      //myString.indexOf(3000);
      String parsedString = myString.substring(5);
      //LOG_VERBOSE("- Command name was => %s\r\n", callbackInfo->tag);
      LOG_VERBOSE("- Delay Command => %s\r\n", (char*) parsedString.c_str());
      delayVal = parsedString.toInt();
      }    
  }
}

void setup() {
  delayVal = 10000;
  Serial.begin(9600);
  oled_setup();

  connect_wifi(WIFI_SSID, WIFI_PASSWORD);
  connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);

  rtc = sensor_rtc_setup();
  bme = sensor_bme_680_setup();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (context != NULL) {
    lastTick = 0;  // set timer in the past to enable first telemetry a.s.a.p
    lastTickDisplay = 0;
 
  }
   DHTbegin();
}

void loop() {

  /* IOT REMINANCE */
  if (isConnected) {

    unsigned long ms = millis();
    if (ms - lastTick > delayVal) {  // send telemetry every 10 seconds
      char msg[64] = {0};
      int pos = 0, errorCode = 0;

      lastTick = ms;
      if (loopId++ % 2 == 0) {  // send telemetry
        //pos = snprintf(msg, sizeof(msg) - 1, "{\"Temperature\": %f}", dhtT);
        errorCode = iotc_send_telemetry(context, msg, pos);
        
        //pos = snprintf(msg, sizeof(msg) - 1, "{\"Humidity\":%f}", dhtH);
        errorCode = iotc_send_telemetry(context, msg, pos);
          
      } else {  // send property
        
      } 
  
      msg[pos] = 0;

      if (errorCode != 0) {
        LOG_ERROR("Sending message has failed with error code %d", errorCode);
      }
    }

    iotc_do_work(context);  // do background work for iotc
  } else {
    iotc_free_context(context);
    context = NULL;
    connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);
  }

/***
 *      ___   _            _                ___   _          _  
 *     |   \ (_) ___ _ __ | | __ _  _  _   / _ \ | | ___  __| | 
 *     | |) || |(_-<| '_ \| |/ _` || || | | (_) || |/ -_)/ _` | 
 *     |___/ |_|/__/| .__/|_|\__,_| \_, |  \___/ |_|\___|\__,_| 
 *                  |_|             |__/                        
 */
/* Display */
  unsigned long ms_disp = millis();
  if (ms_disp - lastTickDisplay > 5000){
    lastTickDisplay = ms_disp;
    screen_display("group3: transport", msg_rtc, msg_bme1, msg_bme2, msg_bme3, msg_hum_temp, msg_photores);
  }

/***
 *      ___             _   _____  _                ___  _           _    
 *     | _ \ ___  __ _ | | |_   _|(_) _ __   ___   / __|| | ___  __ | |__ 
 *     |   // -_)/ _` || |   | |  | || '  \ / -_) | (__ | |/ _ \/ _|| / / 
 *     |_|_\\___|\__,_||_|   |_|  |_||_|_|_|\___|  \___||_|\___/\__||_\_\ 
 *                                                                        
 */
/* RTC */
  unsigned long ms_rtc = millis();
  if (ms_rtc - lastTickRTC > 5000){
    lastTickRTC = ms_rtc;
    DateTime now = rtc.now();
    snprintf(msg_rtc, 75, "%2d/%2d/%4d %2d:%2d:%2d", now.day(),now.month(),now.year(), now.hour(), now.minute(),now.second());
    //snprintf(msg_rtc, 75, "%ld/%ld/%ld %ld:%ld", now.day(),now.month(),now.year(), now.hour(),now.minute());
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
  if (ms_dht - lastTickDHT > 5000){
    lastTickDHT = ms_disp;
    float dhtH = readHumidity();
    float dhtT = readTemperature();
    snprintf(msg_humidity, 75, "h: %3.1f", dhtH);
    snprintf(msg_temp, 75, "t: %3.1f C", dhtT);
    snprintf(msg_hum_temp, 75, "DHT| h:%3.1f%% t:%3.1fC ", dhtH, dhtT);
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
  if (ms_pres - lastTickPhotoResistor > 5000){
    lastTickPhotoResistor = ms_disp;
    int PhotoResValue = analogRead(SENSORPIN);
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
  if (ms_bme - lastTickBME > 5000){
    lastTickBME = ms_disp;
    //float dhtHumidity = dht.readHumidity();
    if (! bme.performReading()) {
        Serial.println("Failed to perform reading :(");
      }
    else {
      //Serial.print("Pressure = "); Serial.print(bme.pressure / 100.0); Serial.println(" hPa");
      //display.print("Pressure: "); display.print(bme.pressure / 100); display.println(" hPa");
      //Serial.print("Gas = "); Serial.print(bme.gas_resistance / 1000.0); Serial.println(" KOhms");
      //display.print("Gas: "); display.print(bme.gas_resistance / 1000.0); display.println(" KOhms");
      snprintf(msg_bme1, 75, "BME| h:%3.1f%% t:%3.1fC", bme.humidity, bme.temperature);
      Serial.println(String(msg_bme1));
      //Serial.print("Temperature = "); Serial.print(bme.temperature); Serial.println(" *C");
      //display.print("Temperature: "); display.print(bme.temperature); display.println(" *C");
      //Serial.print("Humidity = "); Serial.print(bme.humidity); Serial.println(" %");
      //display.print("Humidity: "); display.print(bme.humidity); display.println(" %");
      snprintf(msg_bme2, 75, "BME| P:   %d  hPa", (bme.pressure / 100));
      Serial.print(String(msg_bme2));
      snprintf(msg_bme3, 75, "BME| Gas:%3.2f KOhms", (bme.gas_resistance/1000.0));
      Serial.print(String(msg_bme3));
    }
  }



}
