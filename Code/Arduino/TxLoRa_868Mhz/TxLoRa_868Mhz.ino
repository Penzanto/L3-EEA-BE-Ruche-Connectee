//include
#include <SoftwareSerial.h>
#include <RH_RF95.h>

//set the pin used for the LoRa module
#define rxPin 2
#define txPin 3
SoftwareSerial LoRaPin (rxPin, txPin);
RH_RF95 rf95(LoRaPin);  //give the pin of the loRa module to the RH_RF95 lib


void setup() {
  Serial.begin(9600); //set the serial port baudrate  

  //initialise the LoRa module
  if (!rf95.init())
  {
      Serial.println("LoRa module init fail");
      while(1);
  }
  else
  {
    Serial.println("LoRa module initialised");
  }

  //set the different seting of the emitter
  rf95.setTxPower(5, false);  //range from 5 to 20dBm
  rf95.setModemConfig(4); //set the setModemConfig to "Bw125Cr48Sf4096" ie "Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range"     4
  rf95.setFrequency(868.0); //set the frequency of the LoRa module (write 434 for the 434Mhz module)
}

void loop() {
  uint8_t data[] = "Hello World!";  //make a buffer of byte for the Tx
  //rf95.send(data, sizeof(data));  //send the data throught the LoRa module
  
  delay(3000);
}
