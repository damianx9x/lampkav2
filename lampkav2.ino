/*
  PROJEKT: Inteligentna Lampa Portal S3 (Cylinder 28x7)
  WERSJA: FINALNA (VERIFIED)
*/

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <FastLED.h>

#include "index.h"
#include "melodies.h"
#include "animations.h"
#include "drawing.h"

#define LED_PIN     12
#define BUZZER_PIN  5
#define BTN_PIN     13
#define NUM_LEDS    201 

CRGB leds[NUM_LEDS];
WebServer server(80);
DNSServer dnsServer;

// Globalne stany
bool powerState = true;
uint8_t brightness = 150;
uint8_t currentEffect = 0;
CRGB globalColor = CRGB::Orange;

// Czas
unsigned long timeOffset = 0;
int alarmH = -1, alarmM = -1;
bool alarmEn = false;
int lastMin = -1;

void setup() {
  Serial.begin(115200);
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  
  initDrawing(); 

  WiFi.mode(WIFI_AP);
  WiFi.softAP("Lampa_Portal_S3", "portal1234");
  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/", []() { server.send(200, "text/html", MAIN_PAGE); });
  
  server.on("/set", []() {
    if (server.hasArg("power")) powerState = (server.arg("power") == "1");
    if (server.hasArg("bri")) { brightness = server.arg("bri").toInt(); FastLED.setBrightness(brightness); }
    if (server.hasArg("anim")) currentEffect = server.arg("anim").toInt();
    if (server.hasArg("alarmH")) {
      alarmH = server.arg("alarmH").toInt();
      alarmM = server.arg("alarmM").toInt();
      alarmEn = true;
    }
    if (server.hasArg("stop")) stopMelody();
    server.send(200, "text/plain", "OK");
  });

  server.on("/sync", []() {
    if (server.hasArg("ts")) timeOffset = strtoull(server.arg("ts").c_str(), NULL, 10) - (millis() / 1000);
    server.send(200, "text/plain", "OK");
  });

  server.on("/draw", handleDraw);

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  
  unsigned long unix = (millis() / 1000) + timeOffset;
  int h = (unix / 3600) % 24;
  int m = (unix / 60) % 60;

  if (m != lastMin) {
    if (alarmEn && h == alarmH && m == alarmM) {
      powerState = true;
      startMelody(0);
    }
    lastMin = m;
  }

  playMelodyNonBlocking(BUZZER_PIN);

  if (powerState) {
    if (currentEffect == 100) displayDrawing(leds, NUM_LEDS);
    else runAnimation(currentEffect, leds, NUM_LEDS, globalColor);
    FastLED.show();
  } else {
    FastLED.clear();
    FastLED.show();
  }
}