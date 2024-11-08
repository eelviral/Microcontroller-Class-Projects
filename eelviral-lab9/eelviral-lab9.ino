// Eddie Elvira
// Lab 9
// GPS Tracker

#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>

float latitude , longitude;         // keep track of changing lat long values
const char *SSID =  "iPhone(2)";    // SSID to transmit data to
const char *password =  "Hello123"; // password of SSID

WiFiClient client;
TinyGPSPlus gps;

// Connect to Serial 2
HardwareSerial SerialGPS(2);

void setup()
{
  Serial.begin(115200);
  
  // Wait until the ESP suucessfully connects to the SSID
  WiFi.begin(SSID, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");             
  }
  Serial.println("");
  Serial.println("WiFi connected!");

  // Begin Serial GPS on Serial 2 wires (GPIO 16 and GPIO 17)
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
}
void loop()
{
  // Provide GPS data as long as it exists
  while (SerialGPS.available() > 0) 
  {
    // Decode read GPS data
    if (gps.encode(SerialGPS.read()))
    {
      // Make sure that GPS data is valid and not null, for instance
      if (gps.location.isValid())
      {
        // Display the latitude
        latitude = gps.location.lat();
        Serial.print("Latitude = ");
        Serial.print(String(latitude, 6) + ", ");

        // Display the longitude
        longitude = gps.location.lng();
        Serial.print("Longitude = ");
        Serial.println(String(longitude, 6));
      }
     delay(1000);
     Serial.println();  
    }
  }  
}