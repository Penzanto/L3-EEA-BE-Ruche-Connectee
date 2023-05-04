#ifndef HX711_H
#define HX711_H
#include <Arduino.h>

//ajout du prototype de toutes les fonctions cree dans HX711.cpp
void lectureHX711(uint32_t *valeur_HX711, uint8_t *erreur_HX711, uint8_t pinClk_HX711, uint8_t pinData_HX711);
void miseEnVeilleHX711(uint8_t pinClk_HX711, uint8_t pinData_HX711);

#endif