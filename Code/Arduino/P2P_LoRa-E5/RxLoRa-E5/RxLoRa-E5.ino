//ajout des libraries utile
#include <SoftwareSerial.h> //librarie pour la creation du port serie virtuel

//reglage des pins du module LoRa sur la carte arduino
#define rxPin 5
#define txPin 6
SoftwareSerial SerialLoRa (rxPin, txPin); //creation d un port serie virtuel car la carte arduino ne possede pas assez de port serie

//reglage des parametres de la ruche
#define codeSysteme 0x2C33  //cle utiliser pour reconnaitre notre message


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
  if(initialisaitonLoRa !=1){
    Serial.println("Blocage du programme");
    while(1){}   
  }
}


void loop() {
  //recuperation des information
  uint16_t cleSysteme =0;
  uint8_t codeRuche =0;
  uint8_t typeCapteur =0;
  float valeurCapteur =0;
  ReceptionLoRa_E5_P2P(&cleSysteme, &codeRuche, &typeCapteur, &valeurCapteur, SerialLoRa, Serial);

  //si la cle systeme correspond bien. alors on affiche toutes les valeurs
  if(cleSysteme == codeSysteme){
    Serial.println("Valeur recu:");
    Serial.println("codeSysteme: " + String(cleSysteme));
    Serial.println("codeRuche: " + String(codeRuche));
    Serial.println("indiceCapteur: " + String(typeCapteur));
    Serial.println("valeurCapteur: " + String(valeurCapteur));
    Serial.println("");
  }
}


//-----------------------------------------------------Fonction cree-----------------------------------------------------
void initialisationLoRa_E5_P2P(uint16_t frequence,  String SpreadFactor, uint16_t BandWidth, uint8_t TX_Preamble, uint8_t RX_Preamble, uint8_t Power, String CRC, String Inverted_IQ, String Public_LoRaWAN, Stream &portSerieVirtuel, Stream &portSerieCom, uint8_t *initialisaitonLoRa)
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
      portSerieCom (Stream): nom de la class du port serie de la comunication USB
      initialisaitonLoRa (uint8_t) variable d acquittement de la bonne initialisation du module LoRa 
  */

  //variable local
  String LoRaRecu = "";
  uint16_t timeOut = 0; 
  
  *initialisaitonLoRa = 1;

  //passage du module Lora en "mode TEST"
  if(*initialisaitonLoRa ==1){
    portSerieVirtuel.print("AT+MODE=TEST"); //envoie de la commande
    timeOut = 0;
    while(portSerieVirtuel.available() == 0 & timeOut < 5000) //attente de reception de la commande jusqu'a un timeout de 5sec
    {
      //attente de 10ms et incrementation du compteur de timeout
      delay(10);
      timeOut = timeOut +10;
    }
    LoRaRecu = portSerieVirtuel.readString(); //lecture de la valeur rendu par la recepteur LoRa
    portSerieCom.println(LoRaRecu); //ecriture de la valeur recu
    if(LoRaRecu.indexOf("+MODE: TEST") <= 0)  //si on ne trouve pas le message "+MODE: TEST" il c est passee une erreur durant l envoie/reception de la reponse de la commande
    {
      //passage de la variable d initialisation a false et ecriture d un message d erreur
      *initialisaitonLoRa = 0;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode TEST");   
    }
  }

  //passage du module Lora en "mode STOP" (empeche le module de recevoir des information durant la reprogramation du mode)
  if(*initialisaitonLoRa ==1){
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
      *initialisaitonLoRa = 0;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode STOP"); 
    }
  }
  
  //reglage du LDRO du module Lora
  if(*initialisaitonLoRa ==1){
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
      *initialisaitonLoRa = 0;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode LDRO");
    }
  }

  //configuration des parametres de reception radio du module LoRa
  if(*initialisaitonLoRa ==1){
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
      *initialisaitonLoRa = 0;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode RFCFG");
    }
  }

  //passage du module LoRa en reception continue de valeur hexadecimal
  if(*initialisaitonLoRa ==1){
    portSerieVirtuel.print("AT+TEST=RXLRPKT");
    timeOut = 0;
    while(portSerieVirtuel.available() <= 0 & timeOut < 5000)
    {
      delay(10);
      timeOut = timeOut + 10;
    }
    LoRaRecu = portSerieVirtuel.readString();
    portSerieCom.println(LoRaRecu);
    if(LoRaRecu.indexOf("+TEST: RXLRPKT") <=0)
    {
      *initialisaitonLoRa = 0;
      portSerieCom.println("");
      portSerieCom.println("erreur ecriture mode recepetion");
    }
  }

  //message de fin d initialisation
  if(*initialisaitonLoRa ==1){
    portSerieCom.println("initialisation du module LoRa OK");
    portSerieCom.println("");
  }
  else{
    portSerieCom.println("erreur lors de l initialisation du module Lora");
  }  
}


void ReceptionLoRa_E5_P2P(uint16_t *cleSysteme, uint8_t *idRuche, uint8_t *typeCapteur, float *valeurCapteur, Stream &portSerieVirtuel, Stream &portSerieCom)
{
  /*
    Fonction de recuperation et de traitement des information LoRa pour extraire la cleSysteme, idRuche, typeCapteur et la valeurCapteur
    
    Args:
      cleSysteme (uint16_t): cle utilisee pour dissocier les messages du projet ruche des autres messages sur la frequence 868/433Mhz
      idRuche (uint8_t): numerotation de la ruche, utiliser pour dissocier plusieur ruche entre elle sur la meme frequence
      typeCapteur (uint8_t): numerotaion du capteur utiliser ie savoir si c est un capteur de temperature ou de poid etc...
      valeurCapteur (float): valeur lu par le capteur (ieee754 32bits)
      portSerieVirtuel (Stream): nom de la class du port serie physique du module LoRa
      portSerieCom (Stream): nom de la class du port serie de la comunication USB
  */
  
  //structure de convertion des float en uin32_t (representation ieee754 des floatants)
  typedef union
  {
    float asFloat;
    uint32_t asUint32_t;
  }
  convertStruct;

  //declaration des variables locals
  convertStruct structValue;  //variable de la strucure
  String messageLoraRecu ="";
  uint8_t debutString =0;  

  //traitement des messages seuelement si il y a des messages en attente dans le buffer
  if(portSerieVirtuel.available() != 0){
    messageLoraRecu = portSerieVirtuel.readString();  
    portSerieCom.println(messageLoraRecu);
    //conversion du string recu en differents integer
    debutString = messageLoraRecu.indexOf("TEST: RX ") +10;
    *cleSysteme = strtol(messageLoraRecu.substring(debutString, debutString +4).c_str(), NULL, 16);
    *idRuche = strtol(messageLoraRecu.substring(debutString +4, debutString +6).c_str(), NULL, 16);
    *typeCapteur = strtol(messageLoraRecu.substring(debutString +6, debutString +8).c_str(), NULL, 16);
    //fcontionement un peu different car on doit convertir du ieee754 32bits en string et l arduino ne supporte pas le 32bits (16bits maximum)
    structValue.asUint32_t = strtol(messageLoraRecu.substring(debutString +8, debutString +12).c_str(), NULL, 16) <<16 | strtol(messageLoraRecu.substring(debutString +12, debutString +16).c_str(), NULL, 16);
    *valeurCapteur = structValue.asFloat;
  }
  else{
    *cleSysteme =0;
    *idRuche =0;
    *typeCapteur =0;
    *valeurCapteur =0;
  }
}