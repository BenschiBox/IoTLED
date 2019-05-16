#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
//#include "FS.h"
#include <FastLED.h>
#include <ArduinoJson.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    12
//#define CLK_PIN   4
#define LED_TYPE    WS2813
#define COLOR_ORDER GRB
#define NUM_LEDS    5
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         10   // Default Brightness on ESP startup
#define FRAMES_PER_SECOND  300

uint8_t gBrightness = 20;
uint8_t gHue = 0; // rotating "base color" for rainbow effect

// Network
const char* ssid     = ***REMOVED***;
const char* password = ***REMOVED***;

IPAddress local_IP(***REMOVED***);
IPAddress gateway(***REMOVED***);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String effect1State = "off";
String effect2State = "off";

// Create AsyncWebServer and WebSocket object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
//AsyncWebSocketClient * globalClient = NULL;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if(type == WS_EVT_CONNECT) {
    Serial.println("Websocket client connection received");
    //globalClient = client;
  } else if(type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
    Serial.println("-----------------------");
    //globalClient = NULL;
  } else if(type == WS_EVT_DATA){
    Serial.print("Data received: ");
    String msg = "";
    for(int i=0; i < len; i++) {
      msg += (char)data[i];
    }
    
    Serial.println(msg);
    gBrightness = (uint8_t)atoi(msg.c_str());
    
    //ws.textAll("response from server!");
    //globalClient->text("response from server!");
  }
}

void loadSettings() {
  File settings = SPIFFS.open("/settings.json", "r");
  if (!settings) {
    Serial.println("ERR: Failed to open settings JSON!");
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  StaticJsonDocument<200> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, settings);
  if (error) {
    Serial.println("ERR: Failed to deserialize JSON file!");
  }

  gBrightness = doc["brightness"];

  Serial.print("Loaded brightness: ");
  Serial.println(gBrightness);
  settings.close();
}

void saveSettings() {
  SPIFFS.remove("/settings.json");
  
  File settings = SPIFFS.open("/settings.json", "w");
  if (!settings) {
    Serial.println("ERR: Failed to open settings file for writing!");
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  StaticJsonDocument<200> doc;
  
  doc["brightness"] = gBrightness;
  
  // Serialize JSON to file
  if (serializeJson(doc, settings) == 0) {
    Serial.println("ERR: Failed to write to settings JSON!");
  }

  // Close the file
  settings.close();
}

/* // Replaces HTML placeholder with state values
String processor(const String& var) {
  if (var == "EFFECT1STATE") {
    return effect1State;
  } else if (var == "EFFECT2STATE") {
    return effect2State;
  }
  return "!!!PLACEHOLDER ERROR!!!"; // replaces not (yet) defined placeholder with this
} */

void setup() {
  Serial.begin(115200);

  // -----------------------------   SPIFFS   -----------------------------

  if(!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  /*
  saveSettings();
  Serial.println("Settings saved");
  */

  loadSettings();
  Serial.println("Settings loaded");
  
  // -----------------------------    LEDS    -----------------------------

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  // -----------------------------    WIFI    -----------------------------

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

  // -----------------------------   SERVER   -----------------------------

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route for settings JSON
  server.on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request){
    saveSettings();
    request->send(SPIFFS, "/settings.json", "application/json");
  });  

  // Route for icons
  server.on("/android-chrome-192x192.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/android-chrome-192x192.png", "image/png");
  });
  server.on("/android-chrome-512x512.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/android-chrome-512x512.png", "image/png");
  });
  server.on("/apple-touch-icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-touch-icon.png", "image/png");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });    
  server.on("/favicon-16x16.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon-16x16.png", "image/png");
  });
  server.on("/favicon-32x32.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon-32x32.png", "image/png");
  });
  server.on("/mstile-150x150.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/mstile-150x150.png", "image/png");
  });
  server.on("/safari-pinned-tab.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/safari-pinned-tab.svg", "image/svg");
  });
  server.on("/browserconfig.xml", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/browserconfig.xml", "application/xml");
  });  
  server.on("/site.webmanifest", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/site.webmanifest", "text/webmanifest");
  });    
  
  /* // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to turn effect1 on
  server.on("/effect1on", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("LED: Rainbow on");
    effect1State = "on";
    effect2State = "off";    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to turn effect1 off
  server.on("/effect1off", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("LED: off");
    effect1State = "off";
    effect2State = "off";    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to turn effect2 on
  server.on("/effect2on", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("LED: Solid red on");
    effect1State = "off";
    effect2State = "on";    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to turn effect2 off
  server.on("/effect2off", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("LED: off");
    effect1State = "off";
    effect2State = "off";    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  }); */

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

void loop(){
  /*// Wait for WebSocket event then send
  if(globalClient != NULL && globalClient->status() == WS_CONNECTED) {
      String randomNumber = String(random(0,20));
      globalClient->text(randomNumber);
  }
  delay(1000);*/
  
  // Run selected LED effect
  //if (effect1State == "on") {               // Rainbow
  //  fill_rainbow(leds, NUM_LEDS, gHue, 7);
  //} else if (effect2State == "on") {          // Solid Red
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  //} else if (effect1State == "off" && effect2State == "off") {
  //  fill_solid(leds, NUM_LEDS, CRGB::Black);
  //}

  FastLED.delay(1000/FRAMES_PER_SECOND);
  //EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  FastLED.setBrightness(gBrightness);
  FastLED.show();
}


// Useful for later

// manual restart button (wire to a GPIO)
//WiFi.disconnect(true);
//SPIFFS.format();
//ESP.restart();
