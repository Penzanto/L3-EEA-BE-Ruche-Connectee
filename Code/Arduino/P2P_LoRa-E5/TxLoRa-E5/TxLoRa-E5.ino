//ajout des libraries utile
#include <SoftwareSerial.h> //librarie pour la creation du port serie virtuel

//reglage des pins du module LoRa sur la carte arduino
#define rxPin 5
#define txPin 6
SoftwareSerial SerialLoRa (rxPin, txPin); //creation d un port serie virtuel car la carte arduino ne possede pas assez de port serie

//reglage des parametres de la ruche
#define codeSysteme 0x2C33  //cle utiliser pour reconnaitre notre message
#define codeRuche 0x00 //indice de la ruche
#define SHT31_Temp 0x00  //Code du capteur SHT31 en mode temperature
#define SHT31_Humid 0x01 //Code du capteur SHT31 en mode humidite
#define DHT11_Temp 0x02  //Code du capteur DHT11 en mode temperature
#define DHT11_Humid 0x03 //Code du capteur DHT11 en mode humidite
#define HC_SR04 0x04 //Code du capteur ultrason HC-SR04 
#define Masse 0x05 //Code du capteur de masse


void setup() {
  Serial.begin(9600); //liaison serie usb
  Serial.println("lancement de l application");
  SerialLoRa.begin(9600); //liaison SerieVirtuelle LoRa

  //initialisation du module LoRa
  uint8_t initialisaitonLoRa = 1;
  uint16_t frequence =868; 
  String SpreadFactor ="SF12";
  uint16_t BandWidth =125;
  uint8_t TX_Preamble =12;
  uint8_t RX_Preamble =15;
  uint8_t Power =14;
  String CRC ="ON";
  String Inverted_IQ ="OFF";
  String Public_LoRaWAN ="OFF";
  initialisationLoRa_E5_P2P(frequence, SpreadFactor, BandWidth, TX_Preamble, RX_Preamble, Power, CRC, Inverted_IQ, Public_LoRaWAN, SerialLoRa, Serial, &initialisaitonLoRa);
  //si l'initialisation du module c est mal passe on vient bloquer le programme
  if(initialisaitonLoRa ==1){
    Serial.println("Blocage du programme");
    while(1){}   
  }
}


void loop() {
  float valeurCapteur =-123456.987654321;  //variable de test
  uint8_t receptionLoRa =1; //variablede verification de la bonne emision d un message LoRa
  envoieLoRa_E5_P2P(codeSysteme, codeRuche, SHT31_Humid, valeurCapteur, SerialLoRa, Serial, &receptionLoRa);  //envoie d un message
  //si l'initialisation du module c est mal passe on vient bloquer le programme
  if(receptionLoRa ==1){
    Serial.println("Erreur lors de l emission d un message LoRa");
    Serial.println("Blocage du programme");
    while(1){}   
  }
  
  //delay d attente avant le prochain message
  delay(2000);
}


//-----------------------------------------------------Fonction cree-----------------------------------------------------
void initialisationLoRa_E5_P2P(uint16_t frequence,  String SpreadFactor, uint16_t BandWidth, uint8_t TX_Preamble, uint8_t RX_Preamble, uint8_t Power, String CRC, String Inverted_IQ, String Public_LoRaWAN, Stream &portSerieVirtuel, Stream &portSerieCom, uint8_t *initialisaitonLoRa)
{
  /*
    Fonction d initialisation du module LoRa-E5
    
    Args:
      frequence (uint16_t): frequence d emission du module
      SpreadFactor (String): 
      BandWidth (uint16_t): bande passante 
      TX_Preamble (uint8_t): 
      RX_Preamble (uint8_t): 
      Power (uint8_t): puissance d emission en dBm
      CRC (String): activation du CRC (activation du CRC "ON")
      Inverted_IQ (String): 
      Public_LoRaWAN (string): 
      portSerieVirtuel (Stream): nom de la class du port serie physique du module LoRa
      portSerieCom (Stream): nom de la class du port serie de la comunication USB
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
    portSerieCom.println(LoRaRecu);
    if(LoRaRecu.indexOf("+MODE: TEST") <= 0)
    {
      //passage de la variable d initialisation a false et ecriture d un message d erreur
      *initialisaitonLoRa = 1;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode TEST");   
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
    portSerieCom.println(LoRaRecu);
    if(LoRaRecu.indexOf("+TEST: STOP") <= 0)
    {
      *initialisaitonLoRa = 1;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode STOP");
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
    portSerieCom.println(LoRaRecu);
    if(LoRaRecu.indexOf("+LW: LDRO, ON") <= 0)
    {
      *initialisaitonLoRa = 1;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode LDRO");
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
    portSerieCom.println(LoRaRecu);
    if(LoRaRecu.indexOf("RFCFG F") <=0)
    {
      *initialisaitonLoRa = 1;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode RFCFG");
    }
  }

  //message de fin d initialisation
  if(*initialisaitonLoRa ==0){
    portSerieCom.println("initialisation du module LoRa OK");
    portSerieCom.println("");
  }
  else{
    portSerieCom.println("erreur lors de l initialisation du module Lora");
  }  
}


void envoieLoRa_E5_P2P(uint16_t cleSysteme, uint8_t idRuche, uint8_t typeCapteur, float valeurCapteur, Stream &portSerieVirtuel, Stream &portSerieCom, uint8_t *receptionLoRa)
{
  /*
    Fonction utiliser pour envoyer une donnee en peer to peer entre deux module Lora-E5
    
    Args:
      cleSysteme (uint16_t): cle utilisee pour dissocier les messages du projet ruche des autres messages sur la frequence 868/433Mhz
      idRuche (uint8_t): numerotation de la ruche, utiliser pour dissocier plusieur ruche entre elle sur la meme frequence
      typeCapteur (uint8_t): numerotaion du capteur utiliser ie savoir si c est un capteur de temperature ou de poid etc...
      valeurCapteur (float): valeur lu par le capteur 
      portSerieVirtuel (Stream): nom de la class du port serie physique du module LoRa
      portSerieCom (Stream): nom de la class du port serie de la comunication USB
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
  portSerieCom.println(LoRaRecu); //affichage du message recu pour debuggage
  if(LoRaRecu.indexOf("+TEST: TX DONE") <= 0)
  {
    *receptionLoRa = 1;
  }
}