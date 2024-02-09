const byte LED_PIN = 13;
const byte METER_PIN = A4;

volatile double timerFrequency; // Timer frequency in Hz

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(METER_PIN, INPUT);
  
  Serial.begin(9600);
  
  // Initialize timer with a default frequency of 0.5 Hz
  startTimer(0.5);
}

void loop()
{ 
  // You can add your main loop code here if needed
}

void startTimer(double frequency){
  noInterrupts();
  
  // Set timerFrequency to the provided frequency
  timerFrequency = frequency;
  
  // Calculate and set the correct values for OCR1A and TCCR1B registers
  unsigned long timerPrescaler = 1024; // You can change this value based on your requirements
  
  unsigned long timerTop = F_CPU / (timerPrescaler * timerFrequency) - 1;
  
  // Set the timer control register (TCCR1B)
  TCCR1B = (TCCR1B & ~((1 << CS10) | (1 << CS11) | (1 << CS12))) | (1 << WGM12) | (1 << CS12); // Set the prescaler
  
  // Set the output compare register (OCR1A)
  OCR1A = timerTop;
  
  // Enable the Timer/Counter1 compare match A interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  interrupts();
}

ISR(TIMER1_COMPA_vect){
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
}

void updateTimerFrequency() {
  // Read the value from the potentiometer connected to pin A4
  int potValue = analogRead(METER_PIN);
  
  // Map the potentiometer value to the desired frequency range
  double mappedFrequency = map(potValue, 0, 1023, 0.1, 10.0);
  
  // Update the timer frequency
  startTimer(mappedFrequency);
}
