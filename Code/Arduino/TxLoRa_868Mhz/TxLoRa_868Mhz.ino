//ajout des libraries utile
#include <SoftwareSerial.h> //librarie pour la creation du port serie virtuel
#include <RH_RF95.h>  //librarie utiliser pour comuniquer avec le module LoRa

//reglage des pins du module LoRa sur la carte arduino
#define rxPin 5
#define txPin 6
SoftwareSerial LoRaPin (rxPin, txPin); //creation d un port serie virtuel car la carte arduino ne possede pas assez de port serie
RH_RF95 rf95(LoRaPin);    //envoie du port serie virtuel a la librarie du module loRa RH_RF95 lib


void setup() {
  Serial.begin(9600); //regle la frequence de comunication du port serie de l arduino pour comuniquer avec l ordinateur  

  //initialise le module LoRa
  if (!rf95.init())
  {
      Serial.println("LoRa module init fail");
      while(1);
  }
  else
  {
    Serial.println("LoRa module initialised");
  }

  //reglage des differents parametre de l emeteur
  //rf95.setTxPower(5, false);  //range from 5 to 20dBm
  //rf95.setModemConfig(4); //set the setModemConfig to "Bw125Cr48Sf4096" ie "Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range"     4
  rf95.setFrequency(868.0); //regle la frequence du module LoRa (ecrire ici 434 pour le module 434Mhz)
}

void loop() {
  //Creation du duffer d emission
  uint8_t data[] = "Hello World!";
  rf95.send(data, sizeof(data));  //envoie du message via le module LoRa  
  rf95.waitPacketSent();  //fonction d attente de la fin d emission du module LoRa

  delay(3000);  //delais d attente pour ne pas trop poluer les frequences radio le temp des tests
}
