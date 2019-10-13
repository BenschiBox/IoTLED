#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "string.h"
#include <FastLED.h>
#include <ArduinoJson.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    12
//#define CLK_PIN   4
#define LED_TYPE    WS2813
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         10   // Default Brightness on ESP startup
#define FRAMES_PER_SECOND  300

// Global Effect Variables
uint8_t activeEffect;
uint8_t brightness;
struct effectData {
  uint32_t color1;
} d_effect1;


uint8_t gHue = 0; // rotating "base color" for rainbow effect

// Network
const char* ssid     = "notactuallymywifi";
const char* password = "youwouldliketoknow";

IPAddress local_IP(192, 168, 178, 6);
IPAddress gateway(192, 168, 178, 1);
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
  if (type == WS_EVT_CONNECT) {
    Serial.println("Websocket client connection received");
    //globalClient = client;
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
    Serial.println("-----------------------");
    //globalClient = NULL;
  } else if (type == WS_EVT_DATA) {
    Serial.print("Data received: ");
    String msg = "";
    for (int i = 0; i < len; i++) {
      msg += (char)data[i];
    }
    Serial.println(msg);

    if (msg.startsWith("0x")) {
      String temp = msg.substring(2, 4);
      int type = temp.toInt();
      msg.remove(0, 4);
      char *eptr;

      switch (type) {
        case 1:
          activeEffect = (uint8_t)msg.toInt();
          break;
        case 2:
          msg.remove(0, 1);
          d_effect1.color1 = (uint32_t)strtoul(msg.c_str(), &eptr, 16);
          break;

        default:
          break;
      }
    }

    //brightness = (uint8_t)atoi(msg.c_str());
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

  activeEffect = doc["activeEffect"].as<uint8_t>();
  brightness = doc["brightness"].as<uint8_t>();
  char *eptr; // does nothing
  String temp;
  temp = doc["effect1Data"]["color1"].as<String>();
  temp.remove(0, 1);
  d_effect1.color1 = (uint32_t)strtoul(temp.c_str(), &eptr, 16); // wtf
  
  //Serial.println(d_effect1.color1);
  //Serial.print("Loaded brightness: ");
  //Serial.println(brightness);
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

  doc["activeEffect"] = activeEffect;
  doc["brightness"] = brightness;
  String temp = String(d_effect1.color1, HEX);
  doc["effect1Data"]["color1"] = "#" + temp;
  Serial.println("Saved Color: #" + temp);

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

  if (!SPIFFS.begin(true)) {
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

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route for .js
  server.on("/IoTLED.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/IoTLED.js", "application/javascript");
  });

  // Route for settings JSON
  server.on("/settings.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    saveSettings();
    request->send(SPIFFS, "/settings.json", "application/json");
  });

  // Route for icons
  server.on("/android-chrome-192x192.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/android-chrome-192x192.png", "image/png");
  });
  server.on("/android-chrome-512x512.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/android-chrome-512x512.png", "image/png");
  });
  server.on("/apple-touch-icon.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/apple-touch-icon.png", "image/png");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });
  server.on("/favicon-16x16.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon-16x16.png", "image/png");
  });
  server.on("/favicon-32x32.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon-32x32.png", "image/png");
  });
  server.on("/mstile-150x150.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/mstile-150x150.png", "image/png");
  });
  server.on("/safari-pinned-tab.svg", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/safari-pinned-tab.svg", "image/svg");
  });
  server.on("/browserconfig.xml", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/browserconfig.xml", "application/xml");
  });
  server.on("/site.webmanifest", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/site.webmanifest", "text/webmanifest");
  });

  /* // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
    }); */

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
}

void loop() {
  switch (activeEffect) {
    case 1:
      fill_solid(leds, NUM_LEDS, d_effect1.color1);
      break;

    case 3:
      Fire(55,120,15);
      break;

    default:
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      break;
  }

  FastLED.setBrightness(brightness);
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  //EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

// Useful for later

// manual restart button (wire to a GPIO)
//WiFi.disconnect(true);
//SPIFFS.format();
//ESP.restart();
