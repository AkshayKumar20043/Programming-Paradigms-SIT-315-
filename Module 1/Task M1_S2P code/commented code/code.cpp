const uint8_t BTN_PIN = 2; // In this line, Button is connected to pin 2
const uint8_t LED_PIN = 13; // Here LED is connected to pin 13

uint8_t buttonPrevState = LOW; // It is used to store previous button state
uint8_t ledState = LOW;     // It is used to store state of LED

void setup()
{
  // Here pins are initialized
  pinMode(BTN_PIN, INPUT_PULLUP);  // Here button pin is used as input with pull-up resistor
  pinMode(LED_PIN, OUTPUT);         // Here LED pin is initialized as output
  Serial.begin(9600);              // Serial communication is started
}

void loop()
{
  uint8_t buttonState = digitalRead(BTN_PIN);  // It is used to read current state of the button

  // all the below 4 lines are used for printing 
  Serial.print(buttonState);    
  Serial.print(buttonPrevState); 
  Serial.print(ledState);         
  Serial.println("");            

  if(buttonState != buttonPrevState)
  {
    ledState = !ledState;                    // It changes state of LED
    digitalWrite(LED_PIN, ledState);        // It updates LED state
  }

  buttonPrevState = buttonState;             // It update previous button state

  delay(500);  // delay of 0.5 seconds
}
