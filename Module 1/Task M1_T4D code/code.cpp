int ledPin1 = 6; // LED connected to digital pin 6
int pirPin1 = 3; // PIR sensor 1 connected to digital pin 3
int ledPin2 = 7; // LED connected to digital pin 7
int pirPin2 = 2; // PIR sensor 2 connected to digital pin 2

volatile bool pirState1 = LOW; // PIR sensor 1 state
volatile bool pirState2 = LOW; // PIR sensor 2 state

int ledPinTimer = 8; // Additional LED is connected to digital pin 8 for timer interrupt

void setup()
{
  pinMode(ledPin1, OUTPUT);
  pinMode(pirPin1, INPUT);

  pinMode(ledPin2, OUTPUT);
  pinMode(pirPin2, INPUT);

  pinMode(ledPinTimer, OUTPUT);

  PCMSK0 |= (1 << PCINT3); // Enable PCINT for digital pin 3 (pirPin1)
  PCMSK0 |= (1 << PCINT2); // Enable PCINT for digital pin 2 (pirPin2)
  PCICR |= (1 << PCIE0);    // Enable PCINT0 interrupt

  noInterrupts(); // Disable interrupts during configuration
  TCCR1A = 0;      // Set entire TCCR1A register to 0
  TCCR1B = 0;      // Set entire TCCR1B register to 0
  TCNT1 = 0;       // Initialize counter value to 0
  OCR1A = 15625;    // Set compare match value (1 second at 16 MHz)
  TCCR1B |= (1 << WGM12); // Configure timer in CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024 and start the timer
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare match interrupt
  interrupts(); // Enable global interrupts

  Serial.begin(9600);
}

void loop()
{
  // The loop is empty for applying interrupts
}

ISR(PCINT0_vect)
{
  if (digitalRead(pirPin1) == HIGH)
  {
    pirState1 = HIGH;
  }
  else
  {
    pirState1 = LOW;
  }

  if (digitalRead(pirPin2) == HIGH)
  {
    pirState2 = HIGH;
  }
  else
  {
    pirState2 = LOW;
  }
}

ISR(TIMER1_COMPA_vect)
{
  digitalWrite(ledPinTimer, !digitalRead(ledPinTimer)); // Toggle the state of the LED connected to pin 8
}

void handlePIRState1()
{
  digitalWrite(ledPin1, pirState1);
  Serial.print("PIR Sensor 1 is ");
  Serial.println(pirState1 ? "HIGH" : "LOW");
}

void handlePIRState2()
{
  digitalWrite(ledPin2, pirState2);
  Serial.print("PIR Sensor 2 is ");
  Serial.println(pirState2 ? "HIGH" : "LOW");
}
