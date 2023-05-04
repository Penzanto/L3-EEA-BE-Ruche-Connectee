#include "TxLoRaE5.h"

//-----------------------------------------------Fonction-----------------------------------------------
void initialisationLoRa_E5_P2P(uint16_t frequence,  String SpreadFactor, uint16_t BandWidth, uint8_t TX_Preamble, uint8_t RX_Preamble, uint8_t Power, String CRC, String Inverted_IQ, String Public_LoRaWAN, Stream &portSerieVirtuel, uint8_t *initialisaitonLoRa)
{
  /*
    Fonction d initialisation du module LoRa-E5
    
    Args:
      frequence (uint16_t): frequence d emission du module
      SpreadFactor (String): facteur d empietement
      BandWidth (uint16_t): bande passante 
      TX_Preamble (uint8_t): longeur des datas en emission
      RX_Preamble (uint8_t): longeur des datas en reception
      Power (uint8_t): puissance d emission en dBm
      CRC (String): activation du CRC (activation du CRC "ON")
      Inverted_IQ (String): sens du changement de frequence de la modulation
      Public_LoRaWAN (string): si on veut oui ou non utiliser le reseau LoRaWan
      portSerieVirtuel (Stream): nom de la class du port serie physique du module LoRa
      initialisaitonLoRa (uint8_t) variable d acquittement de la bonne initialisation du module LoRa. si TRUE erreur de l init
  */

  //variable local
  String LoRaRecu = "";
  uint16_t timeOut = 0; 

  *initialisaitonLoRa = 0;
  
  //passage du module Lora en "mode TEST"
  if(*initialisaitonLoRa ==0){
    portSerieVirtuel.print("AT+MODE=TEST");
    timeOut = 0;
    while(portSerieVirtuel.available() == 0 & timeOut < 5000)
    {
      delay(10);
      timeOut = timeOut +10;
    }
    LoRaRecu = portSerieVirtuel.readString();
    if(LoRaRecu.indexOf("+MODE: TEST") <= 0)
    {
      //passage de la variable d initialisation a false et ecriture d un message d erreur
      *initialisaitonLoRa = 1; 
    }
  }
  
  //passage du module Lora en "mode STOP" (empeche le module de recevoir des information durant la reprogramation du mode)
  if(*initialisaitonLoRa ==0){
    portSerieVirtuel.print("AT+TEST=STOP");
    timeOut = 0;
    while(portSerieVirtuel.available() == 0 & timeOut < 5000)
    {
      delay(10);
      timeOut = timeOut +10;
    }
    LoRaRecu = portSerieVirtuel.readString();
    if(LoRaRecu.indexOf("+TEST: STOP") <= 0)
    {
      *initialisaitonLoRa = 1;
    }
  }

  //reglage du LDRO du module Lora
  if(*initialisaitonLoRa ==0){
    portSerieVirtuel.print("AT+LW=LDRO,ON");
    timeOut = 0;
    while(portSerieVirtuel.available() <= 0 & timeOut < 5000)
    {
      delay(10);
      timeOut = timeOut +10;
    }
    LoRaRecu = portSerieVirtuel.readString();
    if(LoRaRecu.indexOf("+LW: LDRO, ON") <= 0)
    {
      *initialisaitonLoRa = 1;
    }
  }

  //configuration des parametres d emission radio du module LoRa
  if(*initialisaitonLoRa ==0){
    portSerieVirtuel.print("AT+TEST=RFCFG," + String(frequence) + "," + SpreadFactor + "," + String(BandWidth) + "," + String(TX_Preamble) + "," + String(RX_Preamble) + "," + String(Power) + "," + CRC + "," + Inverted_IQ + "," + Public_LoRaWAN);
    timeOut = 0;
    while(portSerieVirtuel.available() <= 0 & timeOut < 5000)
    {
      delay(10);
      timeOut = timeOut + 10;
    }
    LoRaRecu = portSerieVirtuel.readString();
    if(LoRaRecu.indexOf("RFCFG F") <=0)
    {
      *initialisaitonLoRa = 1;
    }
  }  
}


void envoieLoRa_E5_P2P(uint16_t cleSysteme, uint8_t idRuche, uint8_t typeCapteur, float valeurCapteur, Stream &portSerieVirtuel, uint8_t *receptionLoRa)
{
  /*
    Fonction utiliser pour envoyer une donnee en peer to peer entre deux module Lora-E5
    
    Args:
      cleSysteme (uint16_t): cle utilisee pour dissocier les messages du projet ruche des autres messages sur la frequence 868/433Mhz
      idRuche (uint8_t): numerotation de la ruche, utiliser pour dissocier plusieur ruche entre elle sur la meme frequence
      typeCapteur (uint8_t): numerotaion du capteur utiliser ie savoir si c est un capteur de temperature ou de poid etc...
      valeurCapteur (float): valeur lu par le capteur 
      portSerieVirtuel (Stream): nom de la class du port serie physique du module LoRa
      receptionLoRa (uint8_t) flag de la bonne emission d un message LoRa. si TRUE erreur de l emission du message
  */
  
  //structure de convertion des float en uin32_t (representation ieee754 des floatants)
  typedef union
  {
    float asFloat;
    uint32_t asUint32_t;
  }
  convertStruct;

  //declaration des variables locals
  convertStruct structValue;
  String StringCleSysteme ="";
  String StringIdRuche ="";
  String StringTypeCapteur ="";
  String StringValeurCapteur ="";  
  String LoRaRecu ="";
  uint16_t timeOut = 0;
  //pointeur flag d erreur
  *receptionLoRa = 0;

  //convertie la valeur de valeurCapteur en hex sur 32bits (ieee754)
  structValue.asFloat = valeurCapteur;
  StringValeurCapteur = String(structValue.asUint32_t, HEX);
  while(StringValeurCapteur.length() < 8)
  {
    StringValeurCapteur = "0" + StringValeurCapteur;
  }

  //convertie la valeur de cleSysteme en hex sur 16bits
  StringCleSysteme = String(cleSysteme, HEX);
  while(StringCleSysteme.length() < 4)
  {
    StringCleSysteme = "0" + StringCleSysteme;
  }

  //convertie la valeur de l'idRuche en hex sur 8bits
  StringIdRuche = String(idRuche, HEX);
  while(StringIdRuche.length() < 2)
  {
    StringIdRuche = "0" + StringIdRuche;
  }

  //convertie la valeur du typeCapteur en hex sur 8bits
  StringTypeCapteur = String(typeCapteur, HEX);
  while(StringTypeCapteur.length() < 2)
  {
    StringTypeCapteur = "0" + StringTypeCapteur;
  }

  //envoie des donnee via le module LoRa
  portSerieVirtuel.print("AT+TEST=TXLRPKT, " + StringCleSysteme + StringIdRuche + StringTypeCapteur + StringValeurCapteur);

  //verification du bon envoie des donnes
  timeOut = 0;
  LoRaRecu = "";
  while(LoRaRecu.indexOf("+TEST: TX DONE") <= 0 & timeOut < 5000)
  {
    //concatenation des value renvoyer par le module LoRa
    if(portSerieVirtuel.available() != 0){
      LoRaRecu = LoRaRecu + portSerieVirtuel.readString();
    }
    //gestion du timeout
    delay(10);
    timeOut = timeOut +10;
  }
  if(LoRaRecu.indexOf("+TEST: TX DONE") <= 0)
  {
    *receptionLoRa = 1;
  }
}