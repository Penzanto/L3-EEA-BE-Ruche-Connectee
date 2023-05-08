#ifndef SHT31_H
#define SHT31_H
#include <Arduino.h>
#include <Wire.h> //librarie standard de arduino

//ajout du prototype de toutes les fonctions cree dans HX711.cpp
void sht31_lectureTempEtHumid(int addrI2C_SHT31, float *temp_SHT31, float *hum_SHT31, uint8_t *Error);
uint8_t sht31_crc8(uint16_t data);

#endif