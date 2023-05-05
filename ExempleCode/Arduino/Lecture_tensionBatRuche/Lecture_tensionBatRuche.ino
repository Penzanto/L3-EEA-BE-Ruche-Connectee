void setup() {
  //initialisation de la comunication serie port com
  Serial.begin(9600);
}

void loop() {
  //lecture et conversion de la valeur lue en tension 
  float voltage = analogRead(A0) * (5.0 / 1023.0);  //l arduino comprend un ADC 10bits avec une valeur max de 5V
  
  //ecriture de la valeur lue 
  Serial.println(String(voltage) + "V");

  delay(500);
}