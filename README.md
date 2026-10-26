# Lampa Portal S3

### Lampa LED z ESP32 i sterowaniem przez przeglądarkę

Projekt łączący elektronikę, oświetlenie adresowalne i interfejs webowy. ESP32 udostępnia własny punkt Wi-Fi oraz panel sterowania lampą.

## Możliwości w kodzie

- Włączanie, wyłączanie i regulacja jasności.
- Wybór animacji LED.
- Tryb rysowania na matrycy.
- Synchronizacja czasu z interfejsu i alarm z melodią.
- Obsługa diod WS2812B przez FastLED.

## Struktura

| Plik | Odpowiedzialność |
| :--- | :--- |
| [lampkav2.ino](lampkav2.ino) | Inicjalizacja sprzętu, Wi-Fi, serwer i główna pętla |
| [index.h](index.h) | Interfejs webowy |
| [animations.h](animations.h) | Efekty świetlne |
| [drawing.h](drawing.h) | Tryb rysowania |
| [melodies.h](melodies.h) | Odtwarzanie melodii |

## Konfiguracja bazowa

Kod główny definiuje 201 diod, sygnał LED na GPIO 12, buzzer na GPIO 5 i wejście przycisku na GPIO 13. Traktuj te wartości jako konfigurację konkretnego prototypu i sprawdź zgodność ze swoją płytką oraz okablowaniem.

## Przygotowanie

1. Otwórz `lampkav2.ino` w Arduino IDE z obsługą ESP32.
2. Zainstaluj bibliotekę FastLED; pozostałe nagłówki sieciowe pochodzą z pakietu ESP32.
3. Dopasuj płytkę, liczbę diod i piny do sprzętu.
4. Ustaw własne dane punktu dostępowego przed wgraniem firmware.
5. Po wgraniu połącz się z Wi-Fi lampy i otwórz adres jej punktu dostępowego.

Zasilanie diod dobierz do rzeczywistego poboru prądu; nie zasilaj całej taśmy z pinu mikrokontrolera. Panel lokalnego prototypu nie jest przeznaczony do bezpośredniego udostępnienia w internecie.

## Status

Repozytorium projektu sprzętowego. Opis odzwierciedla dostępny kod, bez deklaracji aktualnego testu na fizycznej lampie.

---

Autor: [Damian Eron](https://github.com/damianx9x) · [Portfolio](https://github.com/damianx9x/damianx9x)
