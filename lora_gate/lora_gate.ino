/*
 * CONNECTIONS:
 * RN2483 - UNO:
 * TX     - 10
 * RX     - 11
 * 3V3    - 3V3
 * GND    - GND
 *
 */

#include "lora_gate_setup.h"
#include "protocol.h"
#include <string.h>

String response;
String data;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(57600);

  Serial.println("Initiating LoRa");
  lorawan_setup();

  // TO-DO connect to the azure server

  Serial.println("Loop starting");
}

void loop()
{
  // a full gateway is not possible since we cannot receive on more thatn one channel
  // there are a lot of commands to talk with the rn2483 module (https://learn.inside.dtu.dk/d2l/le/content/146042/viewContent/571204/View),
  // but it seems to be inable to allow me to see the mac address for the received message

  // ask the rn2483 to go into continous eceive mode (the 0)
  // it will only be exited once a message is received
  loraSerial.println("radio rx 0"); // wait for 60 seconds to receive

  // we will get two responses from the module
  // this one should be ok, and it means the tranceiver is in receive mode
  response = loraSerial.readStringUntil('\n');
  delay(20);
  if (response.indexOf("ok") == 0)
  {
    // if we are in receive mode, we will wait until we get a message
    response = String("");
    while (response == "")
    {
      response = loraSerial.readStringUntil('\n');
    }

    // the second message can either be "radio_rx" or "radio_err"
    if (response.indexOf("radio_rx") == 0)
    {
      // we will get radio_dx <data>
      // read 24 hex characters
      // ----> why is it 24?
      data = response.substring(10, 34); // what if the response is shorter?
      // char msg[24];
      // for (int i = 0; i < 24; i += 2)
      // {
      //   msg[i / 2] = (strtoul(data[i], NULL, 16) << 4) | strtoul(data[i + 1]), NULL, 16);
      // }
      // String jsonMsg = decodeAlertMessage(data);
      // TO-DO send the message to the azure server
      Serial.println(data);
    }
  }
  else
  {
    // wait until its not busy anymore to try again
    delay(1000);
  }
}
