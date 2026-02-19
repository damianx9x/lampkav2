#ifndef ANIMATIONS_H
#define ANIMATIONS_H
#include <FastLED.h>

#define WIDTH 28  // Obwód
#define HEIGHT 7  // Ilość poziomów (pion)

// Mapowanie dla walca 28x7
int XY(int x, int y) {
  if (y < 0 || y >= HEIGHT) return -1;
  
  // Zawijanie X (efekt nieskończonego obrotu)
  x = x % WIDTH;
  if (x < 0) x += WIDTH;

  int idx = y * WIDTH + x;
  if (idx < 201) return idx;
  return -1;
}

// Funkcja rysująca cyfry do zegara statycznego
void drawDigit(CRGB* leds, int xOffset, int num, CRGB color) {
  // Prosta czcionka 3x5
  const byte digits[10][5] = {
    {0x7,0x5,0x5,0x5,0x7}, {0x2,0x6,0x2,0x2,0x7}, {0x7,0x1,0x7,0x4,0x7}, {0x7,0x1,0x7,0x1,0x7}, {0x5,0x5,0x7,0x1,0x1},
    {0x7,0x4,0x7,0x1,0x7}, {0x7,0x4,0x7,0x5,0x7}, {0x7,0x1,0x2,0x4,0x4}, {0x7,0x5,0x7,0x5,0x7}, {0x7,0x5,0x7,0x1,0x7}
  };
  for(int y=0; y<5; y++) {
    for(int x=0; x<3; x++) {
      if(digits[num][4-y] & (1<<(2-x))) {
        int idx = XY(xOffset + x, y + 1); // +1 by wyśrodkować w pionie
        if(idx != -1) leds[idx] = color;
      }
    }
  }
}

void animStaticClock(CRGB* l, int n, int h, int m) {
  FastLED.clear();
  // HH:MM (szerokość ok 15px)
  drawDigit(l, 6, h/10, CRGB::Green);
  drawDigit(l, 10, h%10, CRGB::Green);
  // Dwukropek
  int d1 = XY(14, 2); if(d1!=-1) l[d1] = CRGB::White;
  int d2 = XY(14, 4); if(d2!=-1) l[d2] = CRGB::White;
  
  drawDigit(l, 16, m/10, CRGB::Green);
  drawDigit(l, 20, m%10, CRGB::Green);
}

void runAnimation(uint8_t e, CRGB* l, int n, CRGB c) {
  static uint8_t gHue = 0;
  EVERY_N_MILLISECONDS(20) { gHue++; }

  switch (e) {
    case 0: // Statyczny kolor
      for(int i=0; i<n; i++) l[i] = c;
      break;

    case 2: // Matrix (Deszcz)
      fadeToBlackBy(l, n, 40);
      if(random8() < 50) {
        int x = random8(WIDTH);
        int idx = XY(x, HEIGHT-1);
        if(idx!=-1) l[idx] = CRGB::Green;
      }
      for(int y=0; y<HEIGHT-1; y++) {
        for(int x=0; x<WIDTH; x++) {
          int cur = XY(x, y);
          int nxt = XY(x, y+1);
          if(cur!=-1 && nxt!=-1) l[cur] = l[nxt];
        }
      }
      break;

    case 17: // Lawa
      for(int i=0; i<n; i++) {
        // Mapowanie 1D na szum 2D dla efektu lawy
        int y = i / WIDTH;
        int x = i % WIDTH;
        uint8_t noise = inoise8(x * 30, y * 30 + millis()/5, millis()/3);
        l[i] = ColorFromPalette(HeatColors_p, noise);
      }
      break;
      
    default: // Tęcza jako domyślny fallback
      fill_rainbow(l, n, gHue, 7);
      break;
  }
}
#endif