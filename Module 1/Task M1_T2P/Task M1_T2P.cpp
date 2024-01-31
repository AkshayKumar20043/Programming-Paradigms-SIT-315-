void setup()
{
  pinMode(6, OUTPUT);
  pinMode(3,INPUT);
  attachInterrupt(digitalPinToInterrupt(3), New_Inter, CHANGE);
  Serial.begin(9600);
}

void loop()
{
  // The loop is empty to implement interrupt 
}
 
 void New_Inter()
 {
   if(digitalRead(3)==HIGH)
  {
    digitalWrite(6,HIGH);
    Serial.println("The LED is on");
    delay(1000);
  }
  else
  {
    digitalWrite(6,LOW);
    Serial.println("The LED is off");
    delay(1000);
  }
 }