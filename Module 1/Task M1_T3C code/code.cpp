int ledPin1 = 6; // LED connected to digital pin 6
int pirPin1 = 3; // PIR sensor 1 connected to digital pin 3
int ledPin2 = 7; // LED connected to digital pin 7
int pirPin2 = 2; // PIR sensor 2 connected to digital pin 2

void setup()
{
  pinMode(ledPin1, OUTPUT);
  pinMode(pirPin1, INPUT);
  attachInterrupt(digitalPinToInterrupt(pirPin1), pirInterrupt1, CHANGE); // interrupt for PIR sensor 1

  pinMode(ledPin2, OUTPUT);
  pinMode(pirPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(pirPin2), pirInterrupt2, CHANGE); // interrupt for PIR sensor 2

  Serial.begin(9600);
}

void loop()
{
  // The loop is empty for applying interrupts
}

void pirInterrupt1()
{
  if (digitalRead(pirPin1) == HIGH)
  {
    digitalWrite(ledPin1, HIGH);
    Serial.println("PIR Sensor 1 is HIGH - LED 1 is HIGH");
    delay(1000);
  }
  else
  {
    digitalWrite(ledPin1, LOW);
    Serial.println("PIR Sensor 1 is LOW - LED 1 is LOW");
    delay(1000);
  }
}

void pirInterrupt2()
{
  if (digitalRead(pirPin2) == HIGH)
  {
    digitalWrite(ledPin2, HIGH);
    Serial.println("PIR Sensor 2 is HIGH - LED 2 is HIGH");
    delay(1000);
  }
  else
  {
    digitalWrite(ledPin2, LOW);
    Serial.println("PIR Sensor 2 is LOW - LED 2 is LOW");
    delay(1000);
  }
}
