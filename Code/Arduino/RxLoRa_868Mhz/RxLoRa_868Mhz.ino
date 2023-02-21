//include
#include <SoftwareSerial.h>
#include <RH_RF95.h>

//set the pin used for the LoRa module
#define rxPin 5
#define txPin 6
SoftwareSerial LoRaPin (rxPin, txPin);
RH_RF95 rf95(LoRaPin);  //give the pin of the loRa module to the RH_RF95 lib


void setup() 
{
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
  //rf95.setTxPower(5, false);  //range from 5 to 20dBm
  //rf95.setModemConfig(4); //set the setModemConfig to "Bw125Cr48Sf4096" ie "Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range"     4
  rf95.setFrequency(868.0); //set the frequency of the LoRa module (write 434 for the 434Mhz module)
}

void loop()
{
   
    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if(rf95.waitAvailableTimeout(3000))
    {
        // Should be a reply message for us now   
        if(rf95.recv(buf, &len))
        {
            Serial.print("got reply: ");
            Serial.println((char*)buf);
        }
        else
        {
            Serial.println("recv failed");
        }
    }
    else
    {
        Serial.println("No reply, is rf95_server running?");
    }
    
    delay(1000);
}


