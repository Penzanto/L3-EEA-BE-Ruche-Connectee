#ifndef TXLORAE5_H
#define TXLORAE5_H
#include <Arduino.h>

//ajout du prototype de toutes les fonctions cree dans TxLoRaE5.cpp
void initialisationLoRa_E5_P2P(uint16_t frequence,  String SpreadFactor, uint16_t BandWidth, uint8_t TX_Preamble, uint8_t RX_Preamble, uint8_t Power, String CRC, String Inverted_IQ, String Public_LoRaWAN, Stream &portSerieVirtuel, uint8_t *initialisaitonLoRa);
void envoieLoRa_E5_P2P(uint16_t cleSysteme, uint8_t idRuche, uint8_t typeCapteur, float valeurCapteur, Stream &portSerieVirtuel, uint8_t *receptionLoRa);

#endif