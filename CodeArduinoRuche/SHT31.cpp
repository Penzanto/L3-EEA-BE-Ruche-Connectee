#include "SHT31.h"

//-----------------------------------------------Fonction-----------------------------------------------
void sht31_lectureTempEtHumid(int addrI2C_SHT31, float *temp_SHT31, float *hum_SHT31, uint8_t *Error)
{
/*
    Fonction de recuperation de la temperature et de l humidite du capteur SHT31 via I2C
    
    Args:
      addrI2C_SHT31  (int): adresse I2C du capteur SHT31
      temp_SHT31 (float): pointeur de la valeur de la temperature du capteur SHT31
      hum_SHT31 (float): pointeur de la valeur de la humidite du capteur SHT31
      Error (uint8_t): pointeur de variable d erreur. si True alors erreur lors de la reception des information du capteur
  */

  //variable local
  //valeur binaire rendu par le capteur SHT31
  uint16_t tempbin_SHT31 = 0;
  uint8_t crcTempRecu_SHT31 = 0;
  uint16_t humbin_SHT31 = 0;
  uint8_t crcHumRecu_SHT31 = 0;

  //pointeur
  //valeur de temperature et humid convertie en °C et %
  *temp_SHT31 = 0;
  *hum_SHT31 = 0;
  //variable d erreur
  *Error = 0;

  //demande de mesure de temperature et d humid
  Wire.beginTransmission(addrI2C_SHT31);
  Wire.write(0x24);
  Wire.write(0x00);
  Wire.endTransmission();

  //delais de mesure
  delay(50);  //delay min 13ms

  //lecture mesure
  Wire.requestFrom(addrI2C_SHT31, 6);
  if(Wire.available() >=6)
  {
    tempbin_SHT31 = Wire.read() << 8 | Wire.read();
    crcTempRecu_SHT31 = Wire.read();
    humbin_SHT31 = Wire.read() << 8 | Wire.read();
    crcHumRecu_SHT31 = Wire.read();
  }
  else
  {
    *Error = 1;
  }
  
  //verification des CRC
  if(crcTempRecu_SHT31 == sht31_crc8(tempbin_SHT31) && crcHumRecu_SHT31 == sht31_crc8(humbin_SHT31) && Error !=1)
  {
    *temp_SHT31 = -45+175*(tempbin_SHT31/65535.0);  //calcul de la temperature
    *hum_SHT31 = 100*(humbin_SHT31/65535.0);  //calcul de l humidite
  }
  else
  {
    *Error = 1;
  }
}

uint8_t sht31_crc8(uint16_t data)
{
 /*
    Fonction de calcul du CRC du capteur SHT31
    
    Args:
    data (uint16_t): data sur laquel on veut calculer le crc
  */

  //variable local
	const uint8_t pol =0x31;  //polynome du crc utilise par la SHT31
	uint8_t crc= 0xFF;  //valeur initial du crc
  
  //decoupage en deux du calcul du crc
  for (int j=0; j<2; j++)
	{
    if(j==0){
      crc = crc ^ data>>8;  //prise en compte que des 8 premiers bits
    }
    else{
      crc = crc ^ data & 0xFF;  //prise en compte que des 8 derniers bits
    }
		
    //decalage binaire et xor du polynome
		for (int i=0; i<8; i++)
		{
      if(crc & 0x80)  //si msb == 1
      {
        crc = (crc << 1) ^ pol;
      }
      else
      {
        crc = crc << 1;
      }
		}
	}
	return crc;
}
