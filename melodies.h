#ifndef MELODIES_H
#define MELODIES_H
#include "Arduino.h"

bool isPlaying = false;
unsigned long lastNoteTime = 0;
int currentMelody = 0;

void startMelody(int i) { 
  currentMelody = i;
  isPlaying = true; 
}

void stopMelody() { 
  isPlaying = false; 
}

void playMelodyNonBlocking(int pin) {
  if (!isPlaying) {
    noTone(pin);
    return;
  }
  
  unsigned long now = millis();
  // Prosty, głośny rytm alarmu (Beep... Beep...)
  // 500ms ciszy, 500ms dźwięku
  if (now - lastNoteTime > 500) {
    // Przełączanie dźwięku
    static bool state = false;
    state = !state;
    if(state) tone(pin, 2000); // 2kHz - dobrze słyszalne
    else noTone(pin);
    
    lastNoteTime = now;
  }
}
#endif