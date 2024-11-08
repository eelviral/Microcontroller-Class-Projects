// Eddie Elvira
// Lab 8
// Web Client

#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "iPhone (2)";
const char* password = "hello123";

// IP address of ESP 32 server with URL paths to LED data (port 26 = red, port 27 = blue)
const char* redServer = "http://172.20.10.9/26";
const char* blueServer = "http://172.20.10.9/27";

String redState;
String blueState;

unsigned long previousMillis = 0;
const long interval = 5000; 

void setup() {
  Serial.begin(115200);

  // Wait until a Wi-Fi connection is made
  WiFi.begin(SSID), password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
     // Check if Wi-Fi is connected
    if(WiFi.status()== WL_CONNECTED )
    {
      // Get state of both red and blue LEDs from the URLs (find out if they're on or off) 
      redState = httpGETRequest(redServer);
      blueState = httpGETRequest(blueServer);
      Serial.println("Red LED: " + redState + " Blue LED: " + blueState);

      // Record the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: " + httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.println("Error code: " + httpResponseCode);
  }
  // Free the extracted resources
  http.end();

  return payload;
}