
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(D5,INPUT_PULLUP);  // Follow the Setup.jpg image schematic to connect the sensor
                             // USE 3.3V instead of 5V as we are using an NodeMCU

}

void loop() {
  // put your main code here, to run repeatedly:
  int tilt =  digitalRead(D5);
  Serial.println(String(tilt));
  delay(500);
}
