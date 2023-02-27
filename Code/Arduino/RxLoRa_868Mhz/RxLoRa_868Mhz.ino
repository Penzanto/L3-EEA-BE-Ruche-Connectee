//ajout des libraries utile
#include <SoftwareSerial.h> //librarie pour la creation du port serie virtuel
#include <RH_RF95.h>  //librarie utiliser pour comuniquer avec le module LoRa

//reglage des pins du module LoRa sur la carte arduino
#define rxPin 5
#define txPin 6
SoftwareSerial LoRaPin (rxPin, txPin); //creation d un port serie virtuel car la carte arduino ne possede pas assez de port serie
RH_RF95 rf95(LoRaPin);  //envoie du port serie virtuel a la librarie du module loRa RH_RF95 lib

//reglage des parametre de la ruche
#define cleSysteme_Valide 0x2C33

void setup() 
{
  Serial.begin(9600); //regle la frequence de comunication du port serie de l arduino pour comuniquer avec l ordinateur  

  //initialisation du module LoRa
  if (!rf95.init())
  {
    Serial.println("Erreur lors de l initialisation du module LoRa");  //message d erreur
    while(1);
  }  
  Serial.println("Module LoRa initialisee");  //message de confirmation du bon foncitonnnement du module LoRa

  //reglage des differents parametre de l emeteur
  rf95.setModemConfig(1); ////modifie le type de reception "Bw500Cr45Sf128" ie: Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
  rf95.setFrequency(868.0); //regle la frequence du module LoRa (ecrire ici 434 pour le module 434Mhz)
}

void loop()
{
  //variable local utilisee
  uint16_t cleSysteme =0;
  uint8_t idRuche =0;
  uint8_t typeCapteur =0;
  uint16_t valeurCapteur =0;
  
  receptionLoRa868_433(10000, &cleSysteme, &idRuche, &typeCapteur, &valeurCapteur); //fonction de reception des informations

  if(cleSysteme == cleSysteme_Valide) //verification de la cle emisse par la ruche
  {
    //affichage de toutes les valeurs envoyer par la ruche
    Serial.println("Nouveau message recu: ");
    Serial.print("cleSysteme: ");
    Serial.println(cleSysteme);
    Serial.print("idRuche: ");
    Serial.println(idRuche);
    Serial.print("typeCapteur: ");
    Serial.println(typeCapteur);
    Serial.print("valeurCapteur: ");
    Serial.println(valeurCapteur);
    Serial.println("");
  }
  else
  {
    //affichage d un message d erreur
    Serial.println("Mauvaise cle recu");
    Serial.println("");
  }
}

void receptionLoRa868_433(uint16_t timeOut, uint16_t *cleSysteme, uint8_t *idRuche, uint8_t *typeCapteur, uint16_t *valeurCapteur)
{
  /*
    Fonction de reception et de conditionnement des information envoyer par la ruche

    Args:
      timeOut (uint16_t): temps d attente de la reception d un message LoRa
      cleSysteme (uint16_t): cle utilisee pour dissocier les messages du projet ruche des autres messages sur la frequence 868/433Mhz
      idRuche (uint8_t): numerotation de la ruche, utiliser pour dissocier plusieur ruche entre elle sur la meme frequence
      typeCapteur (uint8_t): numerotaion du capteur utiliser ie savoir si c est un capteur de temperature ou de poid etc...
      valeurCapteur (uint16_t): valeur lu par le capteur
  */

  //creation d un buffer de reception des messages
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  rf95.waitAvailableTimeout(timeOut);  //attend qu'un message soit disponible jusqu'au timeOut
  
  if(rf95.recv(buf, &len))  //si un message est disponible dans le buffer
  {
    if(len == 6)  //si le message fais bien 6 octes
    {
      //recuperation des differentes information dans le bon format
      *cleSysteme = buf[0]<<8 | buf[1];
      *idRuche = buf[2];
      *typeCapteur = buf[3];
      *valeurCapteur = buf[4]<<8 | buf[5];  
    }
  }
}
