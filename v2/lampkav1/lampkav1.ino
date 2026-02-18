/*
  Projekt: Inteligentna Lampa LED 201 (Portal Edition S3)
  Final Fix: Pełna obsługa jasności, kolorów, budzika i auto-on/off
*/

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <FastLED.h>
#include "index.h"
#include "melodies.h"
#include "animations.h"

// --- PINOUT ---
#define LED_PIN     12
#define BUZZER_PIN  5
#define BTN_PIN     13

#define NUM_LEDS    201
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
WebServer server(80);
DNSServer dnsServer;

// --- STATE ---
bool powerState = true;
uint8_t brightness = 150;
uint8_t currentEffect = 0;
CRGB globalColor = CRGB::Cyan;

// --- TIME & ALARM ---
unsigned long timeOffset = 0;
int alarmHour = -1, alarmMinute = -1;
bool alarmEnabled = false, alarmActive = false;
int alarmMelodyID = 0;

int autoOnH = -1, autoOnM = -1; bool autoOnEn = false;
int autoOffH = -1, autoOffM = -1; bool autoOffEn = false;

// --- BUTTON & MODES ---
enum SpecialMode { MODE_NORMAL, MODE_STATIC_CLOCK };
SpecialMode specialMode = MODE_NORMAL;
unsigned long modeTimer = 0;
int clickCount = 0;
unsigned long lastClickTime = 0;
bool lastBtnState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  FastLED.clear();
  FastLED.show();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("Lampa_Portal_S3", "12345678");
  dnsServer.start(53, "*", WiFi.softAPIP());
  MDNS.begin("portal");

  server.on("/", []() { server.send(200, "text/html", MAIN_PAGE); });
  server.on("/set", handleSet);
  server.on("/sync", handleSync);
  server.onNotFound([]() { server.send(200, "text/html", MAIN_PAGE); });
  server.begin();
  
  Serial.println("System Ready");
}

void handleSet() {
  // Power
  if (server.hasArg("power")) {
    powerState = (server.arg("power") == "1");
    Serial.printf("Power: %d\n", powerState);
  }
  
  // Brightness
  if (server.hasArg("bri")) {
    brightness = server.arg("bri").toInt();
    FastLED.setBrightness(brightness);
    Serial.printf("Brightness: %d\n", brightness);
  }
  
  // Animation
  if (server.hasArg("anim")) {
    currentEffect = server.arg("anim").toInt();
    specialMode = MODE_NORMAL;
    Serial.printf("Effect: %d\n", currentEffect);
  }
  
  // RGB Color
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    globalColor = CRGB(server.arg("r").toInt(), server.arg("g").toInt(), server.arg("b").toInt());
    currentEffect = 10; // Tryb koloru statycznego
    Serial.println("Color Updated");
  }

  // Alarm & Preview
  if (server.hasArg("almSnd")) {
    alarmMelodyID = server.arg("almSnd").toInt();
    if (server.hasArg("preview")) {
       startMelody(alarmMelodyID);
    }
  }

  if (server.hasArg("alarmH")) {
    alarmHour = server.arg("alarmH").toInt();
    alarmMinute = server.arg("alarmM").toInt();
    alarmEnabled = (server.arg("alarmOn") == "1");
    Serial.printf("Alarm Set: %02d:%02d Enabled: %d\n", alarmHour, alarmMinute, alarmEnabled);
  }
  
  // Schedule
  if (server.hasArg("onH")) { 
    autoOnH = server.arg("onH").toInt(); 
    autoOnM = server.arg("onM").toInt(); 
    autoOnEn = (server.arg("onEn") == "1"); 
  }
  if (server.hasArg("offH")) { 
    autoOffH = server.arg("offH").toInt(); 
    autoOffM = server.arg("offM").toInt(); 
    autoOffEn = (server.arg("offEn") == "1"); 
  }
  
  if (server.hasArg("stopAlarm")) {
    stopMelody();
    alarmActive = false;
  }
  
  server.send(200, "text/plain", "OK");
}

void handleSync() {
  if (server.hasArg("ts")) {
    unsigned long ts = strtoull(server.arg("ts").c_str(), NULL, 10);
    timeOffset = ts - (millis() / 1000);
    Serial.printf("Time Sync: %lu\n", ts);
  }
  server.send(200, "text/plain", "OK");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  unsigned long now = millis();

  // Przycisk
  bool btn = digitalRead(BTN_PIN);
  if (btn == LOW && lastBtnState == HIGH) {
    if (now - lastClickTime < 400) clickCount++; else clickCount = 1;
    lastClickTime = now;
    delay(50); // Debounce
  }
  lastBtnState = btn;

  if (clickCount > 0 && (now - lastClickTime > 450)) {
    if (clickCount == 1) {
      if(isMelodyPlaying() || alarmActive) { stopMelody(); alarmActive = false; }
      else powerState = !powerState;
    } else if (clickCount == 2) {
      powerState = true;
      specialMode = MODE_STATIC_CLOCK;
      modeTimer = now;
    }
    clickCount = 0;
  }

  // Czas i Automatyzacja
  static unsigned long lastCheck = 0;
  if (now - lastCheck > 1000) {
    lastCheck = now;
    unsigned long unix = (now / 1000) + timeOffset;
    int h = (unix / 3600) % 24;
    int m = (unix / 60) % 60;
    int s = unix % 60;

    // Logika Alarmu
    if (alarmEnabled && h == alarmHour && m == alarmMinute && !alarmActive && s < 2) {
      alarmActive = true; 
      powerState = true;
      startMelody(alarmMelodyID);
      currentEffect = 14; 
    }
    
    // Auto ON/OFF
    if (autoOnEn && h == autoOnH && m == autoOnM && s < 2) powerState = true;
    if (autoOffEn && h == autoOffH && m == autoOffM && s < 2) powerState = false;
  }

  playMelodyNonBlocking(BUZZER_PIN);

  if (powerState || specialMode != MODE_NORMAL) {
    if (specialMode == MODE_STATIC_CLOCK) {
      unsigned long unix = (now / 1000) + timeOffset;
      animStaticClock(leds, NUM_LEDS, (unix / 3600) % 24, (unix / 60) % 60);
      if (now - modeTimer > 8000) specialMode = MODE_NORMAL;
    } else {
       runAnimation(currentEffect, leds, NUM_LEDS, globalColor);
    }
    FastLED.show();
  } else {
    FastLED.clear();
    FastLED.show();
  }
}