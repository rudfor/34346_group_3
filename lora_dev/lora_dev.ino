/*
 * CONNECTIONS:
 * RN2483 - ESP8266:
 * TX     - D6
 * RX     - D5
 * 3V3    - 3V3
 * GND    - GND
 *
 */

#include "lora_dev_setup.h"
//#include "protocol.h"

String response;
String message;
String data;

String appEUI = "017f";
String devEUI = "ed96";

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);  // Serial communication to PC

  Serial.println("Initiating LoRa");
  //pinMode(7, INPUT);
  //pinMode(6, OUTPUT);

  lorawan_setup();

  // we can use
  // loraSerial.print("mac set appkey ");
  // loraSerial.print("mac set devaddr ");
  // loraSerial.print("mac set deveui ");
  // to sorta mimic lorawan,
  // but i dont know how to read it on the other end

  Serial.println("Loop starting");
}

void loop() {

  // send a hex
  if (true)  // TODO, change to: if there is an alert
  {
    loraSerial.print("radio tx ");
    //Sensors sensor = TEMPERATURE;
    //message = encodeAlertMessage(sensor, true, true, 2047); // TODO change this
    message = "1234";
    loraSerial.print(appEUI);
    loraSerial.print(devEUI);
    loraSerial.println(message);

    // we will get two responses from the rn2483
    // this one should be "ok", and it means parameter is valid
    response = loraSerial.readStringUntil('\n');
    Serial.println(response);

    // this one should be "radio_tx_ok", and it means the transmission was successful
    response = loraSerial.readStringUntil('\n');
    Serial.println(response);
    Serial.println();

    delay(5000);


    //delay(2700);
//
    //loraSerial.println("radio rx 1");
    //delay(20);
//
    //if (response.indexOf("ok") == 0) {
    //  // if we are in receive mode, we will wait until we get a message
    //  response = String("");
    //  while (response == "") {
    //    response = loraSerial.readStringUntil('\n');
    //  }
//
    //  // the second message can either be "radio_rx" or "radio_err"
    //  if (response.indexOf("radio_rx") == 0) {
    //    data = response.substring(10, 34);  // we get the message
    //    Serial.println(data);               // TO-DO do something with the message
    //  }
    //}
//
    //Serial.println();
    //delay(7000);
//
    //loraSerial.println("radio rx 1");
    //delay(20);
//
    //if (response.indexOf("ok") == 0) {
    //  // if we are in receive mode, we will wait until we get a message
    //  response = String("");
    //  while (response == "") {
    //    response = loraSerial.readStringUntil('\n');
    //  }
//
    //  // the second message can either be "radio_rx" or "radio_err"
    //  if (response.indexOf("radio_rx") == 0) {
    //    data = response.substring(10, 34);  // we get the message
    //    Serial.println(data);               // TO-DO do something with the message
    //  }
    //}
  }
}
