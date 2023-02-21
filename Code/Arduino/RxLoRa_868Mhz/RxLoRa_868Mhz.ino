//ajout des libraries utile
#include <SoftwareSerial.h> //librarie pour la creation du port serie virtuel
#include <RH_RF95.h>  //librarie utiliser pour comuniquer avec le module LoRa

//reglage des pins du module LoRa sur la carte arduino
#define rxPin 5
#define txPin 6
SoftwareSerial LoRaPin (rxPin, txPin); //creation d un port serie virtuel car la carte arduino ne possede pas assez de port serie
RH_RF95 rf95(LoRaPin);  //envoie du port serie virtuel a la librarie du module loRa RH_RF95 lib


void setup() 
{
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
  //rf95.setModemConfig(4); //set the setModemConfig to "Bw125Cr48Sf4096" ie "Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range"     4
  rf95.setFrequency(868.0); //regle la frequence du module LoRa (ecrire ici 434 pour le module 434Mhz)
}

void loop()
{
  //Creation du duffer de reception
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  rf95.waitAvailableTimeout(3000);  //attend qu'un message soit disponible
  if(rf95.recv(buf, &len))  //si un message est disponible dans le buffer
  {
      //affichage du message recu
      Serial.print("Message received: ");
      Serial.println((char*)buf);
  }
}