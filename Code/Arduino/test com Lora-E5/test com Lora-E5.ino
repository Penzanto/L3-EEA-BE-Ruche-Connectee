//include
#include <SoftwareSerial.h>

//set the software serial pin
#define rxPin 2
#define txPin 3
SoftwareSerial softSerial (rxPin, txPin);

void setup() {
  //set the software serial pin
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  softSerial.begin(9600); //set the software serial baudrate

  //send a data frame as test
  softSerial.write("AT");
}

void loop() {
}
