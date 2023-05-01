/*
 * CONNECTIONS:
 * RN2483 - ESP8266:
 * TX     - D6
 * RX     - D5
 * 3V3    - 3V3
 * GND    - GND
 * 
 */

#include "lora_gate_setup.h"
#include <string.h>
#include <ESP8266WiFi.h>
#include "C:/Users/gusta/Documents/GitRepos/34346_group_3/lora_gate/iotc/common/string_buffer.h"
#include "C:/Users/gusta/Documents/GitRepos/34346_group_3/lora_gate/iotc/iotc.h"

String response;
String data;

String appEUI;
String devEUI;
String message;
String authAttempt;
bool messageToSend = true;

#define WIFI_SSID "GJJ morotola one"
#define WIFI_PASSWORD "gullejwifi"

const char* SCOPE_ID = "0ne009CC2C0";
const char* DEVICE_ID = "parceltransportmonitor";
const char* DEVICE_KEY = "1UHoAkj2dZl5NVDw6tip2AqbKwmPYJ93UBAKbqIpB3U=";

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo);
#include "connection.h"

bool authenticated = false;
String auth[5];

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

  if (strcmp(callbackInfo->eventName, "Command") == 0) {
    LOG_VERBOSE("- Command name was => %s\r\n", callbackInfo->tag);
  }
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);

  connect_wifi(WIFI_SSID, WIFI_PASSWORD);
  connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);

  Serial.println("Initiating LoRa");
  lorawan_setup();

  auth[3] = "017FED96";

  // TO-DO connect to the azure server

  Serial.println("Loop starting");
}

void loop() {
  // a full gateway is not possible since we cannot receive on more thatn one channel
  // there are a lot of commands to talk with the rn2483 module (https://learn.inside.dtu.dk/d2l/le/content/146042/viewContent/571204/View),
  // but it seems to be inable to allow me to see the mac address for the received message



  // ask the rn2483 to go into continous eceive mode (the 0)
  // it will only be exited once a message is received
  loraSerial.println("radio rx 0");  // wait for 60 seconds to receive

  // we will get two responses from the module
  // this one should be ok, and it means the tranceiver is in receive mode
  response = loraSerial.readStringUntil('\n');
  delay(20);
  if (response.indexOf("ok") == 0) {
    // if we are in receive mode, we will wait until we get a message
    response = String("");
    while (response == "") {
      response = loraSerial.readStringUntil('\n');
    }

    // the second message can either be "radio_rx" or "radio_err"
    if (response.indexOf("radio_rx") == 0) {
      // we will get radio_dx <data>
      // read 24 hex characters
      // ----> why is it 24?
      //data = response.substring(10, 34);  // what if the response is shorter?
      //response
      appEUI = response.substring(10, 14);
      devEUI = response.substring(14, 18);
      authAttempt = appEUI + devEUI;
      authenticated = false;
      Serial.println(response);
      for (int i = 0; i < 6; i++) {
        if (auth[i] == authAttempt) {
          authenticated = true;
        }
      }
      if (authenticated) {

        message = response.substring(18, 24);
        Serial.println(data);
        Serial.println(message);
      }

      //if (messageToSend) {
      //  delay(3000);
      //  {
      //    loraSerial.println("deadbeef");
      //  }
      //}



      // char msg[24];
      // for (int i = 0; i < 24; i += 2)
      // {
      //   msg[i / 2] = (strtoul(data[i], NULL, 16) << 4) | strtoul(data[i + 1]), NULL, 16);
      // }
      // String jsonMsg = decodeAlertMessage(data);
      // TO-DO send the message to the azure server
    }
  } else {
    // wait until its not busy anymore to try again
    delay(1000);
  }
}

