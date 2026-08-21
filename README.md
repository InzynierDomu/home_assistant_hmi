# Home Assistant HMI

A touchscreen panel for controlling Home Assistant — ESP32-S3, 800x480 RGB display + GT911 touch
(the same board as [`range_training_screen`](../range_training_screen)). UI generated with
SquareLine Studio (default/dark/light themes), HA integration over REST API, no MQTT involved.

## Required environment

- **Board**: ESP32-S3, 800x480 RGB panel, GT911 touch (board definition: [`esp32-s3-devkitc-1-myboard.json`](esp32-s3-devkitc-1-myboard.json))
- **Platform**: [PlatformIO](https://platformio.org/)
- **Framework**: Arduino — pinned `espressif32@7.0.1` + `arduino-esp32#2.0.3` (see the comment in `platformio.ini`; the newer pioarduino fork isn't compatible)

## Build

```bash
"C:/Users/markiszy/.platformio/penv/Scripts/pio.exe" run --project-dir "C:/projects/home_assistant_hmi"
```

## Configuration

Wi-Fi and Home Assistant settings live in `include/config.h` — the file is gitignored (it holds
secrets). Before the first build, copy the template and fill it in:

```bash
cp include/config.h.example include/config.h
```

| Field | Description |
|---|---|
| `wifi_ssid` / `wifi_password` | Your Wi-Fi network credentials |
| `ha_host` | IP address or hostname of your HA instance (no `http://`) |
| `ha_port` | Usually `8123` |
| `ha_token` | Long-lived access token — HA → user profile (click your avatar, bottom left) → **Long-Lived Access Tokens** → Create Token |
| `ha_light_entity` | Entity ID of the light toggled by `ui_Button2` (via the `light.toggle` service) |
| `ha_sensor_entity` | Entity ID shown on `ui_Label2` (e.g. a thermometer) |
| `ha_sensor_poll_ms` | How often `ui_Label2` is refreshed, in ms (default `30000`) |

Find entity IDs in HA under **Settings → Devices & Services → Entities**, or directly in
**Developer Tools → States** (filter by device name).

## Home Assistant

- `ui_Button2` (event `HA_action`, `src/ui_events.c`) calls `light.toggle` on `ha_light_entity`.
- `ui_Label2` is refreshed periodically with the state of `ha_sensor_entity` (value + `unit_of_measurement`, if the entity has one).
- REST client: [`include/ha_client.h`](include/ha_client.h) / [`src/ha_client.cpp`](src/ha_client.cpp) — plain HTTP (not HTTPS), synchronous calls from `loop()`, `Connection: close` (no keep-alive, to avoid stale sockets between polls).

## SquareLine Studio export

1. In SquareLine Studio, set target: **Arduino (PlatformIO)**, board: ESP32, resolution 800x480,
   color depth 16 bit (matches `include/lv_conf.h`).
2. Project Export → drop the generated `.c` files into `src/`, `.h` files into `include/`,
   overwriting the existing ones.
3. SquareLine resets `HA_action` in `ui_events.c` to an empty stub on every export — after
   overwriting, add back `#include "ha_client.h"` and `ha_toggle_light();` inside the function body.
4. `pio run` to build.
