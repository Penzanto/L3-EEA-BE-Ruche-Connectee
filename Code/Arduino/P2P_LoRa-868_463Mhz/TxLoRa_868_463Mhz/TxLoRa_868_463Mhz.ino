//ajout des libraries utile
#include <SoftwareSerial.h> //librarie pour la creation du port serie virtuel
#include <RH_RF95.h>  //librarie utiliser pour comuniquer avec le module LoRa

//reglage des pins du module LoRa sur la carte arduino
#define rxPin 5
#define txPin 6
SoftwareSerial LoRaPin (rxPin, txPin); //creation d un port serie virtuel car la carte arduino ne possede pas assez de port serie
RH_RF95 rf95(LoRaPin);    //envoie du port serie virtuel a la librarie du module loRa RH_RF95 lib

//reglage des parametres de la ruche
#define codeSysteme 0x2C33  //cle utiliser pour reconnaitre notre message parmis tous les autres message a la frequence 868/433Mhz
#define codeRuche 0x0 //indice de la ruche
#define SHT31_Temp 0x0  //Code du capteur SHT31 en mode temperature
#define SHT31_Humid 0x1 //Code du capteur SHT31 en mode humidite
#define DHT11_Temp 0x2  //Code du capteur DHT11 en mode temperature
#define DHT11_Humid 0x3 //Code du capteur DHT11 en mode humidite
#define HC_SR04 0x4 //Code du capteur ultrason HC-SR04 
#define Masse 0x5 //Code du capteur de masse

void setup() {
  Serial.begin(9600); //regle la frequence de comunication du port serie de l arduino pour comuniquer avec l ordinateur  

  //initialisation du module LoRa
  if (!rf95.init())
  {
    Serial.println("Erreur lors de l initialisation du module LoRa");  //message d erreur
    while(1);
  }  
  Serial.println("Module LoRa initialisee");  //message de confirmation du bon foncitonnnement du module LoRa

  //reglage des differents parametre de l emeteur
  rf95.setTxPower(5, false);  //regle la puissance d emission, va de 5 a 20dBm
  rf95.setModemConfig(1); //modifie le type d emission "Bw500Cr45Sf128" ie: Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
  rf95.setFrequency(868.0); //regle la frequence du module LoRa (ecrire ici 434 pour le module 434Mhz)
}

void loop() {
  envoieLoRa868_433(codeSysteme, codeRuche, SHT31_Humid, 0xAA);  //fonction d emission des donnees
  delay(5000);  //delais d attente pour ne pas trop poluer les frequences radio le temp des tests
}

void envoieLoRa868_433(uint16_t cleSysteme, uint8_t idRuche, uint8_t typeCapteur, uint16_t valeurCapteur)
{
  /*
    Fonction d emission d une valeur lue par l un des capteurs de la ruche
    
    Args:
      cleSysteme (uint16_t): cle utilisee pour dissocier les messages du projet ruche des autres messages sur la frequence 868/433Mhz
      idRuche (uint8_t): numerotation de la ruche, utiliser pour dissocier plusieur ruche entre elle sur la meme frequence
      typeCapteur (uint8_t): numerotaion du capteur utiliser ie savoir si c est un capteur de temperature ou de poid etc...
      valeurCapteur (uint16_t): valeur lu par le capteur
  */

  //Creation et remplissage du duffer d emission
  uint8_t data[6];
  data[0] = cleSysteme >>8;
  data[1] = cleSysteme & 0xFF;
  data[2] = idRuche;
  data[3] = typeCapteur;
  data[4] = valeurCapteur>>8;
  data[5] = valeurCapteur & 0xFF;

  rf95.send(data, sizeof(data));  //envoie du message via le module LoRa
  rf95.waitPacketSent();  //fonction d attente de la fin d emission du module LoRa
}
