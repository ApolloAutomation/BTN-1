# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Apollo BTN-1 Macro Deck firmware - a 4-button ESP32-C6 based customizable macro keyboard with smart home integration. Two implementations exist:

1. **ESPHome (Primary)** - YAML-based configuration in `ESPHome/`
2. **Zigbee (Experimental)** - Arduino C++ in `ZigbeeBTN/`

## Build Commands

### ESPHome
```bash
# Compile firmware (from repo root)
esphome compile Integrations/ESPHome/BTN-1.yaml

# Upload to device
esphome upload Integrations/ESPHome/BTN-1.yaml

# View logs
esphome logs Integrations/ESPHome/BTN-1.yaml
```

### Zigbee (Arduino IDE)
- Board: ESP32-C6 DevKitM-1
- Requires: Zigbee mode set to "End Device" in Tools menu

## CI/CD

PRs are validated against 3 ESPHome versions (stable, beta, dev) for both:
- `BTN-1.yaml` (full featured)
- `BTN-1_Minimal.yaml` (dashboard import version)

Merges to `main` trigger firmware build and publish to GitHub Pages.

## Architecture

### ESPHome Structure
- `Core.yaml` - Main configuration (all hardware definitions, logic, scripts)
- `BTN-1.yaml` - Imports Core.yaml as package, adds WiFi credentials
- `BTN-1_Minimal.yaml` - For ESPHome Dashboard distribution

Key patterns:
- YAML anchors (`&common_light`) for DRY configuration
- Deep sleep with ext1 wakeup on button GPIOs (2, 4, 5, 6)
- Event-based button handling (click, double_click, triple_click, hold)
- Status LED colors: blue=HA connected, green=WiFi only, yellow=offline

### Hardware Pin Map
| Function | GPIO |
|----------|------|
| Button 1 | 2 |
| Button 2 | 4 |
| Button 3 | 6 |
| Button 4 | 5 |
| Reset Button | 9 |
| WS2812 LEDs | 21 |
| Buzzer | 23 |
| I2C SDA | 1 |
| I2C SCL | 0 |
| Power Latch | 19 |
| Accessory Power | 18 |

### External Components
- `github://ApolloAutomation/esphome-battery-component` - MAX17048 battery sensor

## Version Format

`YY.MM.DD.#` (e.g., 25.12.5.1) - Update in `substitutions.version` in Core.yaml

## Important Behaviors

- **Reset button**: Short press runs test script, 3s hold prevents sleep, 10s hold factory resets
- **OTA Mode**: Controlled by Home Assistant `input_boolean.apollo_ota_mode`
- **Deep sleep**: Default 24h, configurable via "Sleep Duration" number entity
