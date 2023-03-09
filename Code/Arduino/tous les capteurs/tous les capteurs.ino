//ecran LCD
#include <rgb_lcd.h>
#include <Wire.h>
rgb_lcd lcd;

//capteur de masse 
#include "HX711.h"

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const int Calibration_Weight = 1000; //éditer

//capteur temp et humidité SHT31
#include "SHT31.h"
SHT31 sht31 = SHT31();

//Capteur temp&humidité DHT11

#include <DHT.h>
#include "DHT.h"
#define DHTTYPE DHT11 
#define DHTPIN 2     // Port D2


DHT dht(DHTPIN, DHTTYPE);

#if defined(ARDUINO_ARCH_AVR)
    #define debug  Serial

#elif defined(ARDUINO_ARCH_SAMD) ||  defined(ARDUINO_ARCH_SAM)
    #define debug  SerialUSB
#else
    #define debug  Serial
#endif

//capteur de mouvement
#define PIR_MOTION_SENSOR 3

void setup() {
  // put your setup code here, to run once:

//capteur SHT31
sht31.begin();

//ecran LCD
lcd.begin(16,2);
delay(1000);

//capteur de mouvement
 pinMode(PIR_MOTION_SENSOR, INPUT);
    Serial.begin(9600);  


//capteur temp et humidité DHT11
 debug.begin(115200);
    debug.println("DHTxx test!");
    Wire.begin();

    dht.begin();




}

void loop() {
  // put your main code here, to run repeatedly:

  //capteur de mouvement
  if(digitalRead(PIR_MOTION_SENSOR))//detecte s'il y a quelqu'un à proximité
        lcd.println("Il y a quelqu'un");
    else
     lcd.println("Surveille       ");
     delay(1500);
     lcd.clear();


//capteur DHT11

float temp_hum_val[2] = {0}; //changer pin pour adapter au code

if (!dht.readTempAndHumidity(temp_hum_val)) {
        lcd.setCursor(0, 0);
        lcd.print("Hint:");
        lcd.print(temp_hum_val[0]);
        lcd.print(" % ");
        lcd.setCursor(0, 1);
        lcd.print("Tint:");
        lcd.print(temp_hum_val[1]);
        lcd.print(" C  ");
        
    } else {
        lcd.println("Failed");
    }

    delay(1500);
    lcd.clear();
    


//capteur SHT31

  float temp = sht31.getTemperature();
  float hum = sht31.getHumidity();
  lcd.setCursor(0, 0);
  lcd.print("Tout="); 
  lcd.print(temp);
  lcd.println(" C     "); //The unit for  Celsius because original arduino don't support speical symbols
  lcd.setCursor(0, 1);
  lcd.print("Hout="); 
  lcd.print(hum);
  lcd.println("%      "); 
  lcd.println();
  
  delay(1500);
  lcd.clear();
}
