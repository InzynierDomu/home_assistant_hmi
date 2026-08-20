# Home Assistant HMI

Pusty projekt PlatformIO pod tę samą płytkę (ESP32-S3, wyświetlacz RGB 800x480 + dotyk GT911)
co [`range_training_screen`](../range_training_screen), przygotowany pod ekran sterujący
Home Assistant (przyciski + odczyt stanu) — UI generowane w SquareLine Studio.

Zawiera tylko warstwę sprzętową: `LGFX.h` (LovyanGFX, panel RGB), `touch.h` (GT911),
`lv_conf.h` (LVGL 8.3.6) oraz minimalny `main.cpp` inicjalizujący wyświetlacz, LVGL i dotyk.
`src/ui.c` / `include/ui.h` to placeholder — bez logiki Home Assistant/Wi-Fi/API.

## Eksport z SquareLine Studio

1. W SquareLine Studio ustaw target: **Arduino (PlatformIO)**, board: ESP32, rozdzielczość 800x480,
   color depth 16 bit (zgodnie z `include/lv_conf.h`).
2. Project Export → wygenerowane pliki `.c` wrzuć do `src/`, pliki `.h` do `include/`,
   nadpisując istniejące `ui.c` / `ui.h`.
3. `pio run` żeby zbudować.

## Build

```
"C:/Users/markiszy/.platformio/penv/Scripts/pio.exe" run --project-dir "C:/projects/home_assistant_hmi"
```
