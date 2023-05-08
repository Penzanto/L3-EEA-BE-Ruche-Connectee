#include <MemoryFree.h>;
#include <pgmStrToRAM.h>;

//declaration des fichier header cree
#include "HX711.h"
#include "SHT31.h"
#include "TxLoRaE5.h"

//decalration des libraries utilisee
#include <SoftwareSerial.h> //librarie pour la creation du port serie virtuel
#include "rgb_lcd.h"  //librarie "Grove-LCD RGB Backlight" pour l utilisation du l ecran LCD grove
#include "DHT.h" //librarie "Grove Temperature And Humidity" pour l utilisation du capteur DHT11
//utilisation de la lib <Wire.h> dans le fichier SHT31.h

//declaration des pins des capteurs
#define data_HX711 2
#define clk_HX711 3
#define outputPIR 4
#define rxLoRaPin 5
#define txLoRaPin 6
#define pin_DHT11 7

//decalaration des definition
#define addr_SHT31 0x44 //adresse I2C du capteur SHT31

//definition utiliser pour la comunication ruche raspberry pi
#define codeSysteme 0x2C33  //cle utiliser pour reconnaitre notre message
#define indiceRuche 0x01 //indice de la ruche
#define SHT31_Temp 0x00  //Code du capteur SHT31 en mode temperature
#define SHT31_Humid 0x01 //Code du capteur SHT31 en mode humidite
#define DHT11_Temp 0x02  //Code du capteur DHT11 en mode temperature
#define DHT11_Humid 0x03 //Code du capteur DHT11 en mode humidite
#define PIR 0x04 //Code du capteur pir
#define HX711_Masse 0x05 //Code de la jauge de contrainte
#define tensionBat 0x06 //code de la tension de la batterie

//creation d un port serie virtuel car la carte arduino ne possede pas assez de port serie (seulement 1 utiliser pour retourner des valeurs et le debugage)
SoftwareSerial SerialLoRa (rxLoRaPin, txLoRaPin); 

//initialisation de la librarie pour l ecran LCD
rgb_lcd lcd;

//initialisation du pin et du type de capteur utilise
DHT dht(pin_DHT11, DHT11);

void setup() {
  //initialisation des pins du µc
  pinMode(clk_HX711, OUTPUT);
  pinMode(data_HX711, INPUT);
  pinMode(outputPIR, INPUT);

  //initialisation de la comunication serie port com
  Serial.begin(9600);

  //initialisation de la comunication serie LoRa
  SerialLoRa.begin(9600); 

  //initialisation de la comunication I2C
  Wire.begin();

  //initialisation de l ecran LCD avec 16 colones et 2 lignes
  lcd.begin(16, 2);

  //initialisation de la librarie DHT
  dht.begin();

  //initialisation du module LoRa
  uint8_t erreurInit_LoRa = 0;
  uint16_t frequence =868; 
  String SpreadFactor ="SF12";
  uint16_t BandWidth =125;
  uint8_t TX_Preamble =12;
  uint8_t RX_Preamble =15;
  uint8_t Power =14;
  String CRC ="ON";
  String Inverted_IQ ="OFF";
  String Public_LoRaWAN ="OFF";
  initialisationLoRa_E5_P2P(frequence, SpreadFactor, BandWidth, TX_Preamble, RX_Preamble, Power, CRC, Inverted_IQ, Public_LoRaWAN, SerialLoRa, &erreurInit_LoRa);
  //ecriture de l init sur l ecran LCD
  lcd.setCursor(0, 0);
  lcd.print("Info init LoRa :");
  lcd.setCursor(0, 1);
  //verification de l init du module LoRa
  if(erreurInit_LoRa !=1){
    //ecriture de l init OK
    lcd.print("Init LoRa OK ");
  }
  else
  {
    //ecriture de l init KO
    lcd.print("Init LoRa KO !!!");
    while(1){} 
  }
}


void loop() {
  //variable local
  uint32_t valeurBrut_HX711 =0;
  uint8_t detection_PIR =0;
  uint8_t erreur_HX711 =0;
  uint8_t erreur_SHT31 =0;
  uint8_t erreur_LoRa =0;
  uint8_t erreur_LoRaGlobal =0; //variable de verification que toutes les valeurs on etaits emise comme il faut
  
  float masseRuche =0;
  float temp_SHT31 =0;
  float hum_SHT31 = 0;
  float temp_DHT11 =0;
  float hum_DHT11 =0;
  float vBat =0;
  float humTemp_DHT11[2] = {0};
  
  //lecture temp & humid via le capteur DHT11
  dht.readTempAndHumidity(humTemp_DHT11);
  //transfert des donnees sous une forme plus facile a comprendre 
  hum_DHT11 = humTemp_DHT11[0];
  temp_DHT11 = humTemp_DHT11[1];

  //lecture temp & humid via le capteur SHT31
  sht31_lectureTempEtHumid(addr_SHT31, &temp_SHT31, &hum_SHT31, &erreur_SHT31);
  //verification de l erreur de la lecture
  if(erreur_SHT31 == 1)
  {
    temp_SHT31 =0;
    hum_SHT31 =0;
  }

  //lecture de la masse de la ruche
  lectureHX711(&valeurBrut_HX711, &erreur_HX711, clk_HX711, data_HX711);
  //verification de l erreur de la lecture
  if(erreur_HX711 != 1)
  {
    masseRuche = (valeurBrut_HX711*0.00215525)-611.204;
  }
  else
  {
    masseRuche =0;
  }

  //detection capteur PIR
  detection_PIR = digitalRead(outputPIR);

  //lecture tension de la batterie
  vBat = analogRead(A0) * (5.0 / 1023.0);

  //emission de toutes les valeurs des capteur au raspberry pi
  envoieLoRa_E5_P2P(codeSysteme, indiceRuche, DHT11_Temp, temp_DHT11, SerialLoRa, &erreur_LoRa);  //envoie de la temp DHT11
  if(erreur_LoRa == 1)
  {
    erreur_LoRaGlobal ++;
  }
  //
  envoieLoRa_E5_P2P(codeSysteme, indiceRuche, DHT11_Humid, hum_DHT11, SerialLoRa, &erreur_LoRa);  //envoie de l humid DHT11
  if(erreur_LoRa == 1)
  {
    erreur_LoRaGlobal ++;
  }
  //
  envoieLoRa_E5_P2P(codeSysteme, indiceRuche, SHT31_Temp, temp_SHT31, SerialLoRa, &erreur_LoRa);  //envoie de la temp SHT31
  if(erreur_LoRa == 1)
  {
    erreur_LoRaGlobal ++;
  }
  //
  envoieLoRa_E5_P2P(codeSysteme, indiceRuche, SHT31_Humid, hum_SHT31, SerialLoRa, &erreur_LoRa);  //envoie de l humid SHT31
  if(erreur_LoRa == 1)
  {
    erreur_LoRaGlobal ++;
  }
  //
  envoieLoRa_E5_P2P(codeSysteme, indiceRuche, HX711_Masse, masseRuche, SerialLoRa, &erreur_LoRa);  //envoie de la masse
  if(erreur_LoRa == 1)
  {
    erreur_LoRaGlobal ++;
  }
  //
  envoieLoRa_E5_P2P(codeSysteme, indiceRuche, PIR, detection_PIR, SerialLoRa, &erreur_LoRa);  //envoie de la detection PIR
  if(erreur_LoRa == 1)
  {
    erreur_LoRaGlobal ++;
  }
  //
  envoieLoRa_E5_P2P(codeSysteme, indiceRuche, tensionBat, vBat, SerialLoRa, &erreur_LoRa);  //envoie de la tension de la batterie
  if(erreur_LoRa == 1)
  {
    erreur_LoRaGlobal ++;
  }

  //ecriture de toutes les valeurs sur l ecran LCD
  // val interne
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp int: " + String(temp_DHT11) + "C");
  lcd.setCursor(0, 1);
  lcd.print("Humi int: " + String(hum_DHT11) + "%");
  delay(3000);
  //val externe
  if(erreur_SHT31  !=1) //detection d une erreur de communication entre le SHT31 et le µc
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp ext: " + String(temp_SHT31) + "C");
    lcd.setCursor(0, 1);
    lcd.print("Humi ext: " + String(hum_SHT31) + "%");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Message erreur :");
    lcd.setCursor(0, 1);
    lcd.print("Com I2C SHT31 KO");
  }
  delay(3000);
  //capteur PIR
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Detection PIR : ");
  lcd.setCursor(0, 1);
  if(detection_PIR)
  {
    lcd.print("VRAI");
  }
  else
  {
    lcd.print("FAUX");
  }
  delay(3000);
  //Masse
  if(erreur_HX711 !=1) //detection d une erreur de communication entre le HX711 et le µc
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Masse ruche :   ");
    lcd.setCursor(0, 1);
    lcd.print(String(masseRuche) + "g");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Message erreur :");
    lcd.setCursor(0, 1);
    lcd.print("Com serie HX711");
  }
  delay(3000);
  //tension de la batterie
  if(vBat > 3.7)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tension bat :   ");
    lcd.setCursor(0, 1);
    lcd.print(String(vBat) + "V");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tens bat faible:");
    lcd.setCursor(0, 1);
    lcd.print(String(vBat) + "V < 3.70V");
  }
  delay(3000);
  //message erreur LoRa
  if(erreur_LoRaGlobal !=0) //detection d une erreur de comunication entre le µc et la module LoRa
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Message erreur :");
    lcd.setCursor(0, 1);
    lcd.print("Com UART LoRa KO");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Emission LoRa :");
    lcd.setCursor(0, 1);
    lcd.print("      OK !      ");
  }
  delay(3000);

  Serial.println("Free SRAM : " + String(freeMemory()) + "/2238");

  //mise en veille des capteurs
  miseEnVeilleHX711(clk_HX711);
  delay(600000);
}
