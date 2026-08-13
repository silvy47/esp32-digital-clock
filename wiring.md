# Wiring Guide

## Components

- ESP32 dev board (any variant with WiFi, e.g. ESP32-WROOM-32)
- TM1637 4-digit 7-segment display module
- 4 jumper wires
- Micro-USB cable (for power + programming)

## Connections

| TM1637 Pin | ESP32 Pin | Notes |
|---|---|---|
| CLK | GPIO 22 | Clock line |
| DIO | GPIO 21 | Data line |
| VCC | 3.3V | Some modules also accept 5V — check your module's datasheet |
| GND | GND | Common ground |

```
   ESP32                     TM1637 Module
 ┌─────────┐               ┌──────────────┐
 │         │               │              │
 │   3.3V ●───────────────●│ VCC          │
 │         │               │              │
 │    GND ●───────────────●│ GND          │
 │         │               │              │
 │  GPIO22 ●───────────────●│ CLK          │
 │         │               │              │
 │  GPIO21 ●───────────────●│ DIO          │
 │         │               │              │
 └─────────┘               └──────────────┘
```

## Notes

- If your GPIO 21/22 pins are already used by I2C on your specific
  board, any two free digital GPIO pins will work — just update
  `CLK_PIN` and `DIO_PIN` at the top of `src/wifi_clock.ino`.
- The display draws very little current, so it can be powered
  directly from the ESP32's 3.3V pin.
- Double-check your module's silkscreen labels — some clones label
  pins in a different order.
