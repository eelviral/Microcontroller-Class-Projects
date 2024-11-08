// Eddie Elvira
// Lab 7
// Sleep States

#include "Peripherals.h"


// *****THE PORT VALUE MATTERS (GPIO 2 is an RTC port used for external wake ups)
#define RTC_B1 2                                  // port used for reading the button
// *****

#define switched                            true  // value for pressed button
#define triggered                           true  // controls interrupt handler
#define debounce                              100 // delay time (ms) to debounce the push-button
volatile bool led_status = LOW;                   // current state of the LED (LOW = off, HIGH = on)
bool initialized = false;                         // only allow interrupts when setup() finishes

volatile bool interrupt_status = {!triggered};    // determines if interrupt was triggered
volatile int hibernating = false;                 // determines hibernation state

PeripheralSetup *pSetup = nullptr;

void setup() 
{
  pinMode(RTC_B1, INPUT);

  // Attach interrupt for peripheral hibernation
  attachInterrupt(digitalPinToInterrupt(RTC_B1), interruptHandler, RISING);

  // Attach RTC interrupt to GPIO 2 for esp32 deep sleep
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 1);

  initialized = true;

  pSetup = new PeripheralSetup();
}

// Interrupt Service Routine (ISR) for handling interrupts
void interruptHandler()
{
  if (initialized)
  {
    // If no interrupt has been triggered, start a new button read process
    if (interrupt_status == !triggered) 
    {
      // If the button was pressed, run buttonSwitchStatus() and begin debounce cycle
      if (digitalRead(RTC_B1) == HIGH) 
      {
        // Deactivate ISR until buttonSwitchStatus() finishes
        interrupt_status = triggered; 
      }
    }
  }
}

// Returns true if button was switched and false if no switch was detected
bool IRAM_ATTR buttonSwitchStatus()
{
  int button_status;
  static bool switch_pending = false;
  static long int time_elapsed;

  if (interrupt_status == triggered) 
  {
    button_status = digitalRead(RTC_B1);  // read current state of button

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
        interrupt_status = !triggered;              // reopen ISR for business now button on/off/debounce cycle complete
        return switched;                       // button was switched
      }
    }
  }

  return !switched; // button was not pressed or debounce time elapsed
}

void loop()
{
 if (buttonSwitchStatus() == switched)   // if button was switched . . .
  {
    // If hibernating, wake everything up
    if (hibernating) {
      // (We dont need an "esp_deep_sleep_end()" function because
      // esp32 will automatically wake up due to the RTC interrupt)

      Serial.println("Waking ESP32 and peripherals");
      pSetup = new PeripheralSetup();
      hibernating = false;
    }
    // If awake, put everything to deep sleep
    else {
      Serial.println("ESP32 and peripherals hibernating");
      delete pSetup;
      pSetup = nullptr;
      hibernating = true;

      // Begin deep sleep
      delay(1000);
      esp_deep_sleep_start();
    }
  }
}
