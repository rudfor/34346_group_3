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

String cmd;
extern SoftwareSerial loraSerial(10, 11);  // TX, RX

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

  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("sys get ver");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("mac pause");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set mod lora");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set freq 864100500");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set pwr 14");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set sf sf12");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set afcbw 41.7");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set rxbw 125");  // Receiver bandwidth can be adjusted here. Lower BW equals better link budget / SNR (less noise).
  cmd = loraSerial.readStringUntil('\n');    // However, the system becomes more sensitive to frequency drift (due to temp) and PPM crystal inaccuracy.

  loraSerial.println("radio set prlen 8");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set crc on");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set iqi off");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set cr 4/5");  // Maximum reliability is 4/8 ~ overhead ratio of 2.0
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set wdt 60000");  // disable for continuous reception
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set sync 12");
  cmd = loraSerial.readStringUntil('\n');

  loraSerial.println("radio set bw 125");
  cmd = loraSerial.readStringUntil('\n');
}