// Eddie Elvira
// Lab 8
// Web Server

#include <WiFi.h>

#define RED 26
#define BLUE 27

const String redport = String(RED, 10);
const String blueport = String(BLUE, 10);

// Network credentials
const char* ssid = "iPhone (2)";
const char* password = "hello123";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String redState = "off";
String blueState = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

int counterValue = 0;
int counter[2] = {0, 0};

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  // Set outputs to LOW
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /" + redport + "/only") >= 0)
            {
              Serial.println("GPIO " + redport + " on");
              Serial.println("GPIO " + blueport + " off");
              redState = "on";
              blueState = "off";
              digitalWrite(RED, HIGH);
              digitalWrite(BLUE, LOW);
              counter[0] = 1;
              counter[1] = 0;
              counterValue++;
            }
            else if (header.indexOf("GET /" + redport + "/on") >= 0) 
            {
              Serial.println("GPIO " + redport + " on");
              redState = "on";
              digitalWrite(RED, HIGH);
              counter[0] = 1;
              counterValue++;
            }
            else if (header.indexOf("GET /" + redport + "/off") >= 0) 
            {
              Serial.println("GPIO " + redport + " off");
              redState = "off";
              digitalWrite(RED, LOW);
            } 
            else if (header.indexOf("GET /" + blueport + "/on") >= 0) 
            {
              Serial.println("GPIO " + blueport + " on");
              blueState = "on";
              digitalWrite(BLUE, HIGH);
              counter[1] = 1;
              counterValue++;
            }
            else if (header.indexOf("GET /" + blueport + "/off") >= 0) 
            {
              Serial.println("GPIO " + blueport + " off");
              blueState = "off";
              digitalWrite(BLUE, LOW);
            } 
            else if (header.indexOf("GET /all/off") >= 0) 
            {
              Serial.println("GPIO " + redport + " off");
              Serial.println("GPIO " + blueport + " off");
              redState = "off";
              blueState = "off";
              digitalWrite(RED, LOW);
              digitalWrite(BLUE, LOW);
              counter[0] = 0;
              counter[1] = 0;
              counterValue = 0;
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".redON { background-color: #FF0000; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".blueON { background-color: #0000FF; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".ledOFF {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Counter</h1>");
            
            client.println("<p>Counter Value = " + String(counterValue, 10) + " (decimal) = " + counter[0] + counter[1] + " (binary)" + "</p>");
            if (counterValue == 0) {
              client.println("<p><a href=\"/" + blueport + "/on\"><button class=\"button\">+1</button></a></p>");
            } else if (counterValue == 1) {
              client.println("<p><a href=\"/" + redport + "/only\"><button class=\"button\">+1</button></a></p>");
            } else if (counterValue == 2) {
              client.println("<p><a href=\"/" + blueport + "/on\"><button class=\"button\">+1</button></a></p>");
            } else {
              client.println("<p><a href=\"/all/off\"><button class=\"button\">RESET</button></a></p>");
            } 

            client.println("<div>");
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO " + redport + " - State " + redState + "</p>");
            // If the output26State is off, it displays the ON button       
            if (redState=="on") {
              client.println("<p><button class=\"redON\" disabled>ON</button></p>");
            } else {
              client.println("<p><button class=\"redON ledOFF\" disabled>OFF</button></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO " + blueport + " - State " + blueState + "</p>");
            // If the output27State is off, it displays the ON button       
            if (blueState=="on") {
              client.println("<p><button class=\"blueON\" disabled>ON</button></p>");
            } else {
              client.println("<p><button class=\"blueON ledOFF\" disabled>OFF</button></p>");
            }
            client.println("</div>");
            client.println("</body></html>");

            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}