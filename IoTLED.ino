#include "WiFi.h"
#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    12
//#define CLK_PIN   4
#define LED_TYPE    WS2813
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          20
#define FRAMES_PER_SECOND  300


uint8_t gHue = 0; // rotating "base color" for rainbow effect
// Network
const char* ssid     = "***REMOVED***";
const char* password = "***REMOVED***";

// Set web server port number to 80
WiFiServer server(80);

IPAddress local_IP(10, 0, 0, 2);
IPAddress gateway(***REMOVED***);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String effect1State = "off";
String effect2State = "off";


void setup() {
  Serial.begin(115200);

  // -----------------------------   LEDS   -----------------------------

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  // -----------------------------   WIFI   -----------------------------

  // Configure WiFi for Static IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  
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
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
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
            
            // switches LED effects
            if (header.indexOf("GET /effect1/on") >= 0) {
              Serial.println("LED: Rainbow on");
              effect1State = "on";
              effect2State = "off";
            } else if (header.indexOf("GET /effect2/on") >= 0) {
              Serial.println("LED: Solid red");
              effect1State = "off";
              effect2State = "on";
            } else if (header.indexOf("GET /effect1/off") >= 0 || header.indexOf("GET /effect2/off") >= 0) {
              Serial.println("LED: off");
              effect1State = "off";
              effect2State = "off";
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE HTML><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for effect1  
            client.println("<p>Rainbow - Effect " + effect1State + "</p>");
            // If the effect1State is off, it displays the ON button       
            if (effect1State=="off") {
              client.println("<p><a href=\"/effect1/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/effect1/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>Solid red - Effect " + effect2State + "</p>");
            // If the effect2State is off, it displays the ON button       
            if (effect2State=="off") {
              client.println("<p><a href=\"/effect2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/effect2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("");
            
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
      } else {
        delay(50);
        if (!client.available()) {
          break;
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
  
  // Run selected LED effect
  if (effect1State == "on") {               // Rainbow
    fill_rainbow( leds, NUM_LEDS, gHue, 7);
  } else if (effect2State == "on") {          // Solid Red
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  } else if (effect1State == "off" && effect2State == "off") {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }

  FastLED.delay(1000/FRAMES_PER_SECOND);
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  FastLED.show();
}
