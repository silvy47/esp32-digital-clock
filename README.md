# WiFi Digital Clock — ESP32

A low-cost digital clock built with an **ESP32** and a **TM1637
4-digit 7-segment display**. Instead of using a separate RTC
(Real-Time Clock) module with its own battery, the ESP32 uses its
built-in WiFi to fetch accurate time from an **NTP (Network Time
Protocol)** server — the same protocol your phone/laptop use to
auto-set their clocks. This cuts hardware cost and wiring complexity.

## Why this project

- Demonstrates **embedded systems + networking** fundamentals: WiFi
  connectivity on a microcontroller, working with a real-world
  protocol (NTP), and driving external hardware (the display).
- Deliberately **low-cost** — no RTC module needed, just an ESP32 and
  a ~$1-2 display.
- Handles real edge cases (WiFi drop, periodic re-sync) instead of a
  toy "happy path only" version.

## How it works

1. On boot, the ESP32 connects to WiFi using credentials from
   `config/secrets.h`.
2. Once connected, it calls `configTime()` (built into the ESP32
   Arduino core) which fetches the current time from an NTP server
   and sets the board's internal clock.
3. The main loop reads the current time and pushes it to the TM1637
   display every 500ms, blinking the colon to give a "ticking"
   effect.
4. Every hour, it re-syncs with the NTP server to correct for clock
   drift. If WiFi drops, it automatically reconnects and re-syncs.

## Hardware

| Component | Notes |
|---|---|
| ESP32 dev board | Any variant with WiFi |
| TM1637 4-digit 7-segment display | ~$1-2, 2-wire interface |

See [`docs/wiring.md`](docs/wiring.md) for the full pin connection
guide and diagram.

## Project structure

```
esp32-wifi-clock/
├── src/
│   └── wifi_clock.ino          # main sketch
├── config/
│   └── secrets_example.h       # WiFi credentials template
├── docs/
│   └── wiring.md                # wiring guide + diagram
└── README.md
```

## Setup instructions

1. **Install the Arduino IDE** (or use PlatformIO) and add ESP32
   board support: File → Preferences → Additional Board Manager URLs
   → add `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`,
   then install "esp32" via Boards Manager.

2. **Install the display library**: Sketch → Include Library →
   Manage Libraries → search "TM1637Display" (by Avishay Orpaz) →
   Install.

3. **Add your WiFi credentials**:
   ```bash
   cp config/secrets_example.h config/secrets.h
   ```
   Then edit `config/secrets.h` and fill in your WiFi SSID/password.
   This file is gitignored so your credentials never get committed.

4. **Set your timezone** in `src/wifi_clock.ino`:
   ```cpp
   const long gmtOffset_sec = 19800; // IST = UTC+5:30 = 19800 seconds
   ```
   Change this to your timezone's UTC offset in seconds.

5. **Wire up the hardware** per [`docs/wiring.md`](docs/wiring.md).

6. **Upload**: select your ESP32 board + correct COM port in the
   Arduino IDE, then click Upload. Open the Serial Monitor (115200
   baud) to see connection/sync status.

## Ideas to extend

- Add a **DHT11/DHT22 sensor** to also show temperature/humidity.
- Add a **push button** to cycle between 12-hour/24-hour format.
- Add a **buzzer + button** for a simple alarm feature.
- Swap the TM1637 for an **OLED display** to also show the date and
  day of the week.
- Add **deep sleep** between display updates to reduce power draw for
  a battery-powered version.

## License

MIT — see [LICENSE](LICENSE).
