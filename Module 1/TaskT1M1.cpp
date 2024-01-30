void setup()
{
  pinMode(3, INPUT);
  pinMode(6,OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  digitalWrite(6, LOW);
  if(digitalRead(3) == HIGH)
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