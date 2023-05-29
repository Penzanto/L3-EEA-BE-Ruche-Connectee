#include "rgb_lcd.h"  //librarie "Grove-LCD RGB Backlight" pour l utilisation du l ecran LCD grove

rgb_lcd lcd;  //initialisation de la librarie pour l ecran LCD

void setup() {
  lcd.begin(16, 2); //initialisation de l ecran LCD avec 16 colones et 2 lignes
}

void loop() {
  lcd.setCursor(0, 0);  //place le curseur sur le premier caractere de la premiere ligne
  lcd.print("BE Projet :"); //ecriture d un message

  lcd.setCursor(0, 1);  //place le curseur sur le premier caractere de la deuxieme ligne
  lcd.print("Ruche connectee"); //ecriture d un message
}
