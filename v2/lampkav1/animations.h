#ifndef ANIMATIONS_H
#define ANIMATIONS_H
#include <FastLED.h>

#define WIDTH 28
#define HEIGHT 8

// --- MAPOWANIE ---
// Zakładamy podstawowe mapowanie wierszowe. Jeśli ledy są fizycznie pionowe,
// ten wzór może wymagać zmiany na x*HEIGHT+y, ale zachowuję oryginał bo efekty działały.
int XY(int x, int y) { 
  x %= WIDTH; 
  if (y<0 || y>=HEIGHT) return -1; 
  int i = y*WIDTH + x; 
  return (i<201) ? i : -1; 
}

// --- FUNKCJE RYSOWANIA (STATYCZNY ZEGAR) ---
// Nowa funkcja dla 2x klik - Bardziej czytelna na cylindrze

void drawSmallDigit(CRGB* leds, int x, int digit, CRGB color) {
  // Proste cyfry 3x5
  const byte font[10][5] = {
    {0x7,0x5,0x5,0x5,0x7}, // 0
    {0x2,0x6,0x2,0x2,0x7}, // 1
    {0x7,0x1,0x7,0x4,0x7}, // 2
    {0x7,0x1,0x7,0x1,0x7}, // 3
    {0x5,0x5,0x7,0x1,0x1}, // 4
    {0x7,0x4,0x7,0x1,0x7}, // 5
    {0x7,0x4,0x7,0x5,0x7}, // 6
    {0x7,0x1,0x2,0x4,0x4}, // 7
    {0x7,0x5,0x7,0x5,0x7}, // 8
    {0x7,0x5,0x7,0x1,0x7}  // 9
  };
  
  // Centrowanie w pionie (y+1) dla matrycy 8px
  for(int j=0; j<5; j++) {
    for(int i=0; i<3; i++) {
      if(font[digit][j] & (1<<(2-i))) {
        // Dodajemy przesunięcie 'x' oraz małą korektę (i)
        int idx = XY(x + i, j + 1); // j+1 żeby było na środku w pionie
        if(idx != -1) leds[idx] = color;
      }
    }
  }
}

void animStaticClock(CRGB* leds, int n, int h, int m) {
  FastLED.clear();
  
  // Format: HH:MM
  // Ponieważ matryca jest zaokrąglona (cylinder), wyświetlamy to na "przodzie"
  // Zakładając WIDTH 28, środek to około 14.
  // Cyfra ma 3px szerokości + 1px odstępu. Cały czas: 3+1+3+2(dwukropek)+3+1+3 = 16px.
  // Startujemy od x=6 aby wycentrować.
  
  int startX = 6;
  CRGB digitColor = CRGB::Green; // Najlepszy kontrast
  CRGB colonColor = CRGB::White;

  // Godziny
  drawSmallDigit(leds, startX, h/10, digitColor);
  drawSmallDigit(leds, startX+4, h%10, digitColor);
  
  // Dwukropek
  int c1 = XY(startX+8, 2); if(c1!=-1) leds[c1] = colonColor;
  int c2 = XY(startX+8, 4); if(c2!=-1) leds[c2] = colonColor;
  
  // Minuty
  drawSmallDigit(leds, startX+10, m/10, digitColor);
  drawSmallDigit(leds, startX+14, m%10, digitColor);
  
  // Opcjonalnie: powtórzenie na "tyłach" cylindra dla widoczności 360
  int backX = (startX + 14) % WIDTH; // Przesunięcie o połowę obwodu
  // Tutaj można dodać drugą parę, ale user chciał "na jednej ze ścian", żeby było czytelnie.
  // Zostawiamy jedną wyraźną instancję.
}


// --- EFEKTY (Poprawki) ---

// 2. Matrix Rain (Poprawiony wizualnie)
void animMatrix(CRGB* l, int n) {
  fadeToBlackBy(l, n, 40); // Szybsze zanikanie
  for(int x=0; x<WIDTH; x++) {
    if(random8() < 10) { // Losowe "krople"
       int y = random8(HEIGHT);
       // Tylko górna krawędź inicjuje nowe
       if (y == 0) {
         int idx = XY(x, y);
         if(idx!=-1) l[idx] = CRGB::Green;
       }
    }
  }
  // Przesuwanie w dół (symulacja)
  // W adresowaniu paskowym trudne bez bufora 2D, więc robimy "iskry"
  int count = random8(3);
  for(int i=0; i<count; i++) {
     int x = random8(WIDTH);
     int y = random8(HEIGHT);
     int idx = XY(x, y);
     if(idx!=-1) l[idx] = CRGB(0, 255, 0);
  }
}

// --- STANDARDOWE EFEKTY (Z pliku usera) ---

void animDNA(CRGB* l, int n) { 
  fadeToBlackBy(l, n, 40); 
  for(int y=0; y<HEIGHT; y++) {
    int x1 = beatsin8(20, 0, WIDTH-1, 0, y*32);
    int x2 = beatsin8(20, 0, WIDTH-1, 0, y*32+128);
    int i1=XY(x1,y), i2=XY(x2,y);
    if(i1!=-1) l[i1]=CHSV(y*20, 255, 255); if(i2!=-1) l[i2]=CHSV(y*20+128, 255, 255);
  }
}

void animEqualizer(CRGB* l, int n) {
  fadeToBlackBy(l, n, 60); // Bardziej dynamiczny
  for(int i=0; i<4; i++) {
    int h = beatsin8(40 + i*10, 0, HEIGHT); // Zamiast random, użyj beatsin dla płynności
    for(int y=0; y<h; y++) { int idx=XY(i*7+3, y); if(idx!=-1) l[idx]=CHSV(i*50, 255, 255); }
  }
}

void animStranger(CRGB* l, int n) {
  uint8_t pulse = beatsin8(15, 20, 80); 
  for (int i = 0; i < n; i++) {
    l[i] = CHSV(0, 255, pulse / 2); 
    uint8_t noise = inoise8(i * 15, millis() / 5);
    if (noise > 190) l[i].fadeToBlackBy(100);
  }
  if (random8() < 15) { 
    int col = random8(WIDTH);
    for (int y = 0; y < HEIGHT; y++) {
      int idx = XY(col, y);
      if (idx != -1) {
        l[idx] = CRGB::Cyan;
        int left = XY(col-1, y); if(left != -1) l[left] += CRGB(0, 40, 80);
        int right = XY(col+1, y); if(right != -1) l[right] += CRGB(0, 40, 80);
      }
    }
  }
  if (random16() < 120) l[random16(n)] = CRGB::White;
  fadeToBlackBy(l, n, 30);
}

// --- NOWE EFEKTY 3D ---
// Skopiowane z oryginału dla zachowania funkcjonalności
void animSnow(CRGB* l, int n) {
  fadeToBlackBy(l, n, 20);
  uint32_t ms = millis();
  int pileHeight = (ms / 5000) % (HEIGHT/2); // Wolniejsze narastanie
  for(int y=0; y<HEIGHT; y++) { if (y < pileHeight) { for(int x=0; x<WIDTH; x++) { int idx = XY(x, y); if(idx!=-1) l[idx] = CRGB(200, 230, 255); } } }
  if (random8() < 120) { int idx = XY(random8(WIDTH), HEIGHT - 1); if(idx!=-1) l[idx] = CRGB::White; }
  // Spadanie
  if (ms % 100 < 30) {
     for(int i=0; i<5; i++) {
       int rx = random8(WIDTH);
       int ry = random8(HEIGHT - 1) + 1;
       int idx = XY(rx, ry);
       int target = XY(rx, ry-1);
       if(idx!=-1 && target!=-1 && l[idx]) { l[target] = l[idx]; l[idx] = CRGB::Black; }
     }
  }
}

void animMagma(CRGB* l, int n) {
  for(int x=0; x<WIDTH; x++) {
    for(int y=0; y<HEIGHT; y++) {
      int idx = XY(x, y);
      if(idx != -1) {
        uint8_t noise = inoise8(x * 30, y * 30 + millis()/5, millis() / 3);
        l[idx] = ColorFromPalette(HeatColors_p, noise);
      }
    }
  }
}

void animVortex(CRGB* l, int n) {
  fadeToBlackBy(l, n, 40);
  uint8_t wave = beat8(30); 
  for(int y=0; y<HEIGHT; y++) {
    int x_target = (wave + (y * 4)) % WIDTH; 
    int idx = XY(x_target, y);
    if(idx != -1) l[idx] = CRGB::Green; 
    int tail = XY(x_target-1, y); if(tail!=-1) l[tail] = CRGB(0, 100, 50);
  }
}

void animScanner(CRGB* l, int n) {
  fadeToBlackBy(l, n, 15); 
  int pos = (millis() / 50) % WIDTH;
  for(int y=0; y<HEIGHT; y++) {
    int idx = XY(pos, y);
    if(idx != -1) l[idx] = CRGB::Blue; 
    if(random8() < 20) l[idx] = CRGB::White;
  }
}

void animRadioactive(CRGB* l, int n) {
  fadeToBlackBy(l, n, 10);
  if(random8() < 60) {
    int idx = XY(random8(WIDTH), random8(HEIGHT));
    if(idx != -1) l[idx] = (random8() > 128) ? CRGB::Green : CRGB(0, 255, 100);
  }
}

// --- STEROWNIK ANIMACJI ---
void runAnimation(uint8_t e, CRGB* l, int n, CRGB c) {
  switch(e) {
    case 0: fill_solid(l, n, CRGB(255,140,20)); break; // Cieplejszy domyślny
    case 2: animMatrix(l, n); break;
    case 10: fill_solid(l,n,c); break;
    case 11: animDNA(l,n); break;
    case 13: animEqualizer(l,n); break;
    case 14: animScanner(l,n); break;
    case 15: animStranger(l, n); break;
    case 16: animSnow(l, n); break;
    case 17: animMagma(l, n); break;
    case 18: animVortex(l, n); break;
    case 19: animScanner(l, n); break; // Powtórka dla listy
    case 20: animRadioactive(l, n); break;
    default: animMagma(l,n); break;
  }
}
#endif