//ajout des libraries utile
#include <SoftwareSerial.h> //librarie pour la creation du port serie virtuel

//reglage des pins du module LoRa sur la carte arduino
#define rxPin 5
#define txPin 6
SoftwareSerial LoRaPin (rxPin, txPin); //creation d un port serie virtuel car la carte arduino ne possede pas assez de port serie

//reglage des parametres de la ruche
#define codeSysteme 0x2C33  //cle utiliser pour reconnaitre notre message
#define codeRuche 0x0 //indice de la ruche
#define SHT31_Temp 0x0  //Code du capteur SHT31 en mode temperature
#define SHT31_Humid 0x1 //Code du capteur SHT31 en mode humidite
#define DHT11_Temp 0x2  //Code du capteur DHT11 en mode temperature
#define DHT11_Humid 0x3 //Code du capteur DHT11 en mode humidite
#define HC_SR04 0x4 //Code du capteur ultrason HC-SR04 
#define Masse 0x5 //Code du capteur de masse


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
