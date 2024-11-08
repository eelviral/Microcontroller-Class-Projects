// Eddie Elvira
// Lab 4
// Display with Interrupts

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

#define B1 23                                     // the port used for reading the pushbutton (GPIO 23)
#define switched                            true  // value for pressed button
#define triggered                           true  // controls interrupt handler
#define debounce                              100  // delay time (ms) to debounce the push-button
volatile bool led_status = LOW;                   // current state of the LED (LOW = off, HIGH = on)
bool initialized = false;                         // only allow interrupts when setup() finishes

volatile bool interrupt_status = {!triggered};    // determines if interrupt was triggered
volatile int message_num = 1;                     // determines current message number

void setup() {
  pinMode(B1, INPUT);
  attachInterrupt(digitalPinToInterrupt(B1), interruptHandler, RISING);
  initialized = true;

  int I2C_SCL = 22;
  int I2C_SDA = 21;
  
  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // // Show initial display buffer contents on the screen --
  // // the library initializes this with an Adafruit splash screen.
  // display.display();
  // delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

void drawHello(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(12345);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xAAAAAAAA, HEX);

  display.display();
  delay(200);
}

void drawWorld(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("World!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(67890);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(200);
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
      if (digitalRead(B1) == HIGH) 
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
    // If current messsage is 1, draw "Hello"
    if (message_num == 1) {
      drawHello();
      message_num = 2;
    }
    // If current message is 2, draw "World!"
    else {
      drawWorld();
      message_num = 1;
    }
  }
}
