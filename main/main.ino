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

int delayVal;
int lastTickDisplay;
int lastTickDHT;
int lastTickRTC;

 
char msg_humidity[75];
char msg_temp[75];
char msg_hum_temp[75];
char msg_serial[75];
char msg_azure[75];
char msg_rtc[75];

RTC_DS1307 rtc;

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
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (context != NULL) {
    lastTick = 0;  // set timer in the past to enable first telemetry a.s.a.p
    lastTickDisplay = 0;
 
  }
   DHTbegin();
}

void loop() {

float dhtH = readHumidity();
float dhtT = readTemperature();

  if (isConnected) {

    unsigned long ms = millis();
    if (ms - lastTick > delayVal) {  // send telemetry every 10 seconds
      char msg[64] = {0};
      int pos = 0, errorCode = 0;

      lastTick = ms;
      if (loopId++ % 2 == 0) {  // send telemetry
        pos = snprintf(msg, sizeof(msg) - 1, "{\"Temperature\": %f}",
                       dhtT);
        errorCode = iotc_send_telemetry(context, msg, pos);
        
        pos = snprintf(msg, sizeof(msg) - 1, "{\"Humidity\":%f}",
                       dhtH);
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

  unsigned long ms_disp = millis();
  if (ms_disp - lastTickDisplay > 5000){
    lastTickDisplay = ms_disp;
    screen_display("group3: transport", msg_rtc, msg_azure, msg_humidity, msg_temp, msg_hum_temp);
  }

  unsigned long ms_rtc = millis();
  if (ms_rtc - lastTickRTC > 5000){
    lastTickRTC = ms_rtc;
    DateTime now = rtc.now();
    snprintf(msg_rtc, 75, "%2d/%2d/%4d %2d:%2d:%2d", now.day(),now.month(),now.year(), now.hour(), now.minute(),now.second());
    //snprintf(msg_rtc, 75, "%ld/%ld/%ld %ld:%ld", now.day(),now.month(),now.year(), now.hour(),now.minute());
    Serial.print("Date & Time: ");
    Serial.println(msg_rtc);
    /* Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC); */
    //screen_display("proj:", msg_serial, msg_azure, msg_humidity, msg_temp);
  }


  unsigned long ms_dht = millis();
  if (ms_dht - lastTickDHT > 5000){
    lastTickDHT = ms_disp;
    //float dhtHumidity = dht.readHumidity();

    snprintf(msg_humidity, 75, "h: %3.1f", dhtH);
    // Read temperature in Celsius, for Fahrenheit use .readTempF()
    //float dhtTempC = dht.readTempC();
    snprintf(msg_temp, 75, "t: %3.1f C", dhtT);
    snprintf(msg_hum_temp, 75, "h: %3.1f %% | t: %3.1f C", dhtH, dhtT);
    //client.publish(outTopic, msg_temp);
    //client.publish(outTopic, msg_humidity);
    Serial.print(F("Humidity: ")); Serial.print(String(dhtH)); Serial.print(F(" [%]\t"));
    Serial.print(F("Temp: ")); Serial.print(dhtT); Serial.println(F(" [C]"));
  }


}
