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

String response;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);  // Serial communication to PC

  Serial.println("Initing LoRa");
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

  loraSerial.print("radio tx ");
  // send a hex
  loraSerial.println("6F6F676C7920626F6F676C79");

  // we will get two responses from the rn2483
  // this one should be "ok", and it means parameter is valid
  response = loraSerial.readStringUntil('\n');
  Serial.println(response);

  // this one should be "radio_tx_ok", and it means the transmission was successful
  response = loraSerial.readStringUntil('\n');
  Serial.println(response);

  delay(2000);
}
