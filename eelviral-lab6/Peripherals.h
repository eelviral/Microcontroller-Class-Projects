#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobot_MAX30102.h>

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
class PeripheralSetup
{
  public:
    PeripheralSetup() {
      Wire.begin(I2C_SDA, I2C_SCL);
      Serial.begin(115200);

      // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
      if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }

      while (!particleSensor.begin()) {
        Serial.println("MAX30102 was not found");
        delay(1000);
      }
      particleSensor.sensorConfiguration(/*ledBrightness=*/50, /*sampleAverage=*/SAMPLEAVG_4, \
                            /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_100, \
                            /*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);

      // Clear the buffer
      display.clearDisplay();
    }

    ~PeripheralSetup()
    {
      display.clearDisplay();
    }

    void displaySensorData(void) 
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println(F("Wait about four seconds"));
      particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, /**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);
      
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.print(F("heartRate="));
      display.print(heartRate, DEC);
      display.print(F(", heartRateValid="));
      display.print(heartRateValid, DEC);
      display.print(F("; SPO2="));
      display.print(SPO2, DEC);
      display.print(F(", SPO2Valid="));
      display.println(SPO2Valid, DEC);
    }
  private:
    int I2C_SCL = 17;
    int I2C_SDA = 4;

    int32_t SPO2; //SPO2
    int8_t SPO2Valid; //Flag to display if SPO2 calculation is valid
    int32_t heartRate; //Heart-rate
    int8_t heartRateValid; //Flag to display if heart-rate calculation is valid 
    DFRobot_MAX30102 particleSensor;
};