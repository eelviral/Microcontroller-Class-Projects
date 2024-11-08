#define B1 12                                     // the port used for reading the pushbutton (GPIO 2)
#define LED 2                                     // the port used for activating the LED (GPIO 4)

#define switched                            true  // value for pressed button
#define triggered                           true  // controls interrupt handler
#define debounce                              200 // delay time (ms) to debounce the push-button

volatile bool led_status = LOW;                   // current state of the LED (LOW = off, HIGH = on)
bool initialized = false;                         // only allow interrupts when setup() finishes

volatile bool interrupt_status = {!triggered};    // determines if interrupt was triggered


// Interrupt Service Routine (ISR) for handling interrupts
void interruptHandler()
{
  if (initialized)
  {
    // If no interrupt has been triggered, start a new button read process
    if (interrupt_status == !triggered) 
    {
      // If the button was pressed, run buttonSwitchStatus() and begin debounce cycle
      if (digitalRead(B1) == HIGH) 
      {
        // Deactivate ISR until buttonSwitchStatus() finishes
        interrupt_status = triggered; 
      }
    }
  }
}

// Returns true if button was switched and false if no switch was detected
bool buttonSwitchStatus()
{
  int button_status;
  static bool switch_pending = false;
  static long int time_elapsed;

  if (interrupt_status == triggered) 
  {
    button_status = digitalRead(B1);  // read current state of button

    if (button_status == HIGH) 
    {
      switch_pending = true;          // button release is now pending
      time_elapsed = millis();        // start timer for debounce checking
    }

    if (switch_pending && button_status == LOW) 
    {
      if (millis() - time_elapsed > debounce)  // button was pressed so check for elapsed debounce time
      {
        switch_pending = false;                // reset variable for the next switch cycle
        interrupt_status = !triggered;         // reopen ISR for business now button on/off/debounce cycle complete
        return switched;                       // button was switched
      }
    }
  }

  return !switched; // button was not pressed or debounce time elapsed
}

void setup()
{
  pinMode(B1, INPUT);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(B1), interruptHandler, FALLING);
  initialized = true;
}

void loop()
{
  if (buttonSwitchStatus() == switched)   // if button was switched . . .
  {
    led_status = HIGH - led_status;       // toggle stored LED state
    digitalWrite(LED, led_status);        // write this state to the LED
  }
}
