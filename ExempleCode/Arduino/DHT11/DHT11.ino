//ajout des lib utilise
#include "DHT.h" //librarie "Grove Temperature And Humidity" pour l utilisation du capteur DHT11

//definition du pin utilise par le capteur DHT11
#define pin_DHT11 7

//initialisation du pin et du type de capteur utilise
DHT dht(pin_DHT11, DHT11);

void setup() {
  //initialisaiton de la liaison serie via le port usb
  Serial.begin(9600);

  //initialisation de la librarie DHT
  dht.begin();
}

void loop() {
  //variable local
  float humTemp_DHT11[2] = {0};

  //lecture de la valeur d humidite et de temperature
  dht.readTempAndHumidity(humTemp_DHT11);
  
  //ecriture des valeurs
  Serial.println("");
  Serial.println("Humidity: " + String(humTemp_DHT11[0]) + "%");
  Serial.println("Temperature: " + String(humTemp_DHT11[1]) + "C");

  delay(1000);
}
