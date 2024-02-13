const int motionSensorPin = 3;
const int soilMoistureSensorPin = 2;  
const int motionLedPin = 6;  // LED for motion sensor
const int moistureLedPin = 7;  // LED for soil moisture sensor

void setup() {
  pinMode(motionLedPin, OUTPUT);
  pinMode(moistureLedPin, OUTPUT);
  pinMode(motionSensorPin, INPUT);
  pinMode(soilMoistureSensorPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(motionSensorPin), motionInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(soilMoistureSensorPin), moistureInterrupt, CHANGE);

  Serial.begin(9600);
}

void loop() {
  // The loop is empty to implement interrupts
}

void motionInterrupt() {
  int motionState = digitalRead(motionSensorPin);

  if (motionState == HIGH) {
    digitalWrite(motionLedPin, HIGH);
    Serial.print("Motion Detected! LED is on. ");
    delay(1000);
  } else {
    digitalWrite(motionLedPin, LOW);
    Serial.println("No motion detected. LED is off.");
    delay(1000);
  }
}

void moistureInterrupt() {
  int moistureState = digitalRead(soilMoistureSensorPin);

  if (moistureState == HIGH) {
    digitalWrite(moistureLedPin, HIGH);
    Serial.println("Soil Moisture Detected! Moisture LED is on.");
    delay(1000);
  } else {
    digitalWrite(moistureLedPin, LOW);
    Serial.println("Soil Moisture Absent. Moisture LED is off.");
    delay(1000);
  }
}
