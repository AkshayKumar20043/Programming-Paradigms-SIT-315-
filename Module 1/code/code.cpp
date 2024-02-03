const int motionSensorPin = 3;
const int temperatureSensorPin = A1;
const int ledPin = 6;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(motionSensorPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(motionSensorPin), NewInter, CHANGE);

  Serial.begin(9600);
}

void loop() {
  // The loop is empty to implement interrupt
}

void NewInter() {
  int motionState = digitalRead(motionSensorPin);
  int temperatureValue = analogRead(temperatureSensorPin);
  float voltage = (temperatureValue / 1024.0) * 5.0; // It convert analog reading to voltage
  float temperatureCelsius = (voltage - 0.5) * 100; // It convert voltage to Celsius

  if (motionState == HIGH) {
    digitalWrite(ledPin, HIGH);
    Serial.print("The LED is on");
    Serial.print("Temperature: ");
    Serial.print(temperatureCelsius);
    Serial.println(" °C");
    delay(1000);
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("The LED is off");
    delay(1000);
  }
}
