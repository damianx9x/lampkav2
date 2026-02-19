#ifndef DRAWING_H
#define DRAWING_H
#include <FastLED.h>

#define DW 28
#define DH 7

CRGB drawBuf[DW * DH];

void initDrawing() {
  for(int i=0; i<(DW*DH); i++) drawBuf[i] = CRGB::Black;
}

// Funkcja wywoływana z poziomu serwera WWW
void handleDraw() {
  extern WebServer server;
  if (server.hasArg("clear")) {
    initDrawing();
  } else if (server.hasArg("x")) {
    int x = server.arg("x").toInt();
    int y = server.arg("y").toInt();
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();
    
    // Zabezpieczenie zakresów
    if(x >= 0 && x < DW && y >= 0 && y < DH) {
      drawBuf[y * DW + x] = CRGB(r, g, b);
    }
  }
  server.send(200, "text/plain", "OK");
}

void displayDrawing(CRGB* leds, int numLeds) {
  extern int XY(int x, int y);
  for(int y=0; y<DH; y++) {
    for(int x=0; x<DW; x++) {
      int idx = XY(x, y);
      if(idx != -1 && idx < numLeds) {
        leds[idx] = drawBuf[y * DW + x];
      }
    }
  }
}
#endif