//declaration des pin du capteur
#define data_HX711 2
#define clk_HX711 3


void setup() {
  //initialisation des pin du µc
  pinMode(clk_HX711, OUTPUT);
  pinMode(data_HX711, INPUT);
  //initialisation de la comunication serie
  Serial.begin(9600);
}


void loop() {
  //variable local
  uint32_t valeur_HX711 = 0;
  uint8_t erreur_HX711 =0;

  //lecture de la valeur 
  lectureHX711(&valeur_HX711, &erreur_HX711, clk_HX711, data_HX711);

  //verification de l erreur de la lecture
  if(erreur_HX711 != 1)
  {
    //correction de la valeur rendu par le capteur
    Serial.println("Masse = " + String((valeur_HX711*0.00215525)-611.204) + "g");
  }
  else
  {
    //ecriture d un message d erreur
    Serial.println("erreur lors de la lecture");
  }

  delay(100);
  // //mise en veille du capteur pendant 3 secondes
  // miseEnVeilleHX711(clk_HX711);
  // delay(3000);
}


//----------------------------------------Fonction----------------------------------------
void lectureHX711(uint32_t *valeur_HX711, uint8_t *erreur_HX711, uint8_t pinClk_HX711, uint8_t pinData_HX711)
{
  /*
    Fonction de lecture d une valeur du capteur HX711
    
    Args:
      valeur_HX711 (uint32_t): valeur brut lu par le capteur
      erreur_HX711 (uint8_t): flag d erreur de lecture de la valeur du capteur (true veut dire qu il y a eu une erreur durant la lecture)
      pinClk_HX711 (uint8_t): numero du pin de la clock du capteur sur le µc
      pinData_HX711 (uint8_t):  numero du pin de la data du capteur sur le µc
  */

  //variable local
  uint16_t timeOut =0;
  
  //mise a zero des pointeurs
  *valeur_HX711 =0;
  *erreur_HX711 =0;
  
  //envoie de la premiere trame de demande de conversion avec un gain de 128
  for(int i =0; i<25; i++)
  {
    digitalWrite(pinClk_HX711, HIGH);
    digitalWrite(pinClk_HX711, LOW);
  }

  //attente de la conversion du capteur avec un timeout de securitee
  timeOut =0;
  while(digitalRead(pinData_HX711)!=0 & timeOut < 1000)
  {
    delay(10);
    timeOut = timeOut +10;
  }

  //verification que le capteur a bien fini de faire ca conversion
  if(digitalRead(pinData_HX711)==0)
  {
    *valeur_HX711 = 0;
    //envoie des trame de clock pour relire la data du capteur
    for(int i =0; i<25; i++)
    {
      digitalWrite(pinClk_HX711, HIGH);
      *valeur_HX711 = *valeur_HX711 << 1 | digitalRead(pinData_HX711);
      digitalWrite(pinClk_HX711, LOW);
    }
  }
  else
  {
    *erreur_HX711 =1;
  }
}

void miseEnVeilleHX711(uint8_t pinClk_HX711)
{
  /*
    Fonction de mise en veille du capteur HX711
    
    Args:
      pinClk_HX711 (uint8_t): numero du pin de la clock du capteur sur le µc
  */
  
  digitalWrite(pinClk_HX711, HIGH);
}
