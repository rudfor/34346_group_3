#include "SDCard.h"

/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

File myFile;

void logSD(String msg_title, String msg_line1, String msg_line2, String msg_line3, String msg_line4, String msg_line5, String msg_line6, String msg_line7) {
  myFile = SD.open("sensors.log", FILE_WRITE);
  if (myFile) {
    myFile.print(msg_title.c_str());
    myFile.print(msg_line1.c_str());
    myFile.print(msg_line2.c_str());
    myFile.print(msg_line3.c_str());
    myFile.print(msg_line4.c_str());
    myFile.print(msg_line5.c_str());
    myFile.print(msg_line6.c_str());
    myFile.println(msg_line7.c_str());
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening sensors.log");
  }
}

void setupSD() {
  // Open serial communications and wait for port to open:
  /***
   *      ___        _  _    _        _  _            _    _              _____          _   
   *     |_ _| _ _  (_)| |_ (_) __ _ | |(_) ___ __ _ | |_ (_) ___  _ _   |_   _|___  ___| |_ 
   *      | | | ' \ | ||  _|| |/ _` || || ||_ // _` ||  _|| |/ _ \| ' \    | | / -_)(_-<|  _|
   *     |___||_||_||_| \__||_|\__,_||_||_|/__|\__,_| \__||_|\___/|_||_|   |_| \___|/__/ \__|
   *                                                                                         
   */
  Serial.print("Initializing SD card...");

  if (!SD.begin(15)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  /***
   *      ___  ___                 _  _          _             _   
   *     / __||   \  __ __ __ _ _ (_)| |_  ___  | |_  ___  ___| |_ 
   *     \__ \| |) | \ V  V /| '_|| ||  _|/ -_) |  _|/ -_)(_-<|  _|
   *     |___/|___/   \_/\_/ |_|  |_| \__|\___|  \__|\___|/__/ \__|
   *                                                               
   */
  // if the file opened okay, write to it:

  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  /***
   *      ___  ___                       _   _             _   
   *     / __||   \   _ _  ___  __ _  __| | | |_  ___  ___| |_ 
   *     \__ \| |) | | '_|/ -_)/ _` |/ _` | |  _|/ -_)(_-<|  _|
   *     |___/|___/  |_|  \___|\__,_|\__,_|  \__|\___|/__/ \__|
   *                                                           
   */
  // re-open the file for reading:
  myFile = SD.open("sensors.log");
  if (false && myFile) {
    Serial.println("sensors.log:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    myFile.close();
    // if the file didn't open, print an error:
    // Serial.println("error opening sensors.log");
  }
  /***
   *      ___                            _                      _             
   *     / __| ___  _ _   ___ ___  _ _  | |    ___  __ _  __ _ (_) _ _   __ _ 
   *     \__ \/ -_)| ' \ (_-</ _ \| '_| | |__ / _ \/ _` |/ _` || || ' \ / _` |
   *     |___/\___||_||_|/__/\___/|_|   |____|\___/\__, |\__, ||_||_||_|\__, |
   *                                               |___/ |___/          |___/ 
   */

}
