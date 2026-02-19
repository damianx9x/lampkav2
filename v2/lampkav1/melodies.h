#ifndef MELODIES_H
#define MELODIES_H

#include "Arduino.h"

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define REST     0

struct Note { int frequency; int duration; };
struct Melody { const Note* notes; int length; const char* name; };

const Note tune_alarm[] = {{NOTE_C5, 100}, {REST, 50}, {NOTE_C5, 100}, {REST, 50}, {NOTE_C5, 100}, {REST, 500}};
const Note tune_morning[] = {{NOTE_C4, 200}, {NOTE_E4, 200}, {NOTE_G4, 200}, {NOTE_C5, 400}};
const Note tune_coin[] = {{NOTE_B5, 100}, {NOTE_E5, 400}};
const Note tune_siren[] = {{NOTE_A4, 300}, {NOTE_C5, 300}, {NOTE_A4, 300}, {NOTE_C5, 300}};
const Note tune_phone[] = {{NOTE_F5, 50}, {NOTE_A5, 50}, {NOTE_F5, 50}, {NOTE_A5, 50}, {REST, 500}};
const Note tune_future[] = {{NOTE_C6, 50}, {NOTE_G5, 50}, {NOTE_E5, 50}, {NOTE_C5, 50}};

Melody melodies[] = {
{tune_alarm, 6, "Klasyk"}, {tune_morning, 4, "Poranek"}, {tune_coin, 2, "Mario Coin"},
{tune_siren, 4, "Syrena"}, {tune_phone, 5, "Telefon"}, {tune_future, 4, "SciFi"}
};

int currentNoteIndex = 0;
unsigned long noteStartTime = 0;
bool isPlaying = false;
int currentMelodyIdx = 0;
int pinBuzzer = -1;

void startMelody(int index) {
if(index < 0 || index >= 6) index = 0;
currentMelodyIdx = index;
currentNoteIndex = 0;
isPlaying = true;
noteStartTime = millis();
}

void stopMelody() {
isPlaying = false;
if(pinBuzzer != -1) noTone(pinBuzzer);
}

bool isMelodyPlaying() { return isPlaying; }

void playMelodyNonBlocking(int pin) {
pinBuzzer = pin;
if (!isPlaying) return;
Melody m = melodies[currentMelodyIdx];
if (currentNoteIndex >= m.length) { currentNoteIndex = 0; return; }
Note n = m.notes[currentNoteIndex];
unsigned long now = millis();
if (now - noteStartTime < (unsigned long)n.duration) {
if (n.frequency > 0) tone(pin, n.frequency); else noTone(pin);
} else {
noTone(pin);
currentNoteIndex++;
noteStartTime = now;
}
}

#endif