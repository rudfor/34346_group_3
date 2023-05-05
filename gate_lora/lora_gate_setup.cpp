/*
 * CONNECTIONS:
 * RN2483 - ESP8266:
 * TX     - D6
 * RX     - D5
 * 3V3    - 3V3
 * GND    - GND
 * 
 */

#include <SoftwareSerial.h>
#include <Arduino.h>

extern SoftwareSerial loraSerial(12, 14);  // TX, RX

void lorawan_setup() {

  loraSerial.begin(9600);  // Serial communication to RN2483
  loraSerial.setTimeout(1000);

  String response = "";
  while (response == "") {
    delay(1000);
    loraSerial.write((byte)0x00);
    loraSerial.write(0x55);
    loraSerial.println();
    loraSerial.println("sys get ver");
    response = loraSerial.readStringUntil('\n');
  }

  loraSerial.listen();

  response = loraSerial.readStringUntil('\n');


  loraSerial.println("sys get ver");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("mac pause");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set mod lora");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set freq 864100500");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set pwr 14");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set sf sf12");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set afcbw 41.7");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set rxbw 125");     // Receiver bandwidth can be adjusted here. Lower BW equals better link budget / SNR (less noise).
  response = loraSerial.readStringUntil('\n');  // However, the system becomes more sensitive to frequency drift (due to temp) and PPM crystal inaccuracy.


  loraSerial.println("radio set prlen 8");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set crc on");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set iqi off");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set cr 4/5");  // Maximum reliability is 4/8 ~ overhead ratio of 2.0
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set wdt 60000");  // disable for continuous reception
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set sync 12");
  response = loraSerial.readStringUntil('\n');


  loraSerial.println("radio set bw 125");
  response = loraSerial.readStringUntil('\n');
}