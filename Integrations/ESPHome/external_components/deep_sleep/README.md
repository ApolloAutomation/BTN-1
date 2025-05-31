# Custom Deep Sleep Component for ESP32-C6

This is a custom ESPHome external component that fixes the ext1 wakeup functionality for ESP32-C6 devices.

## Problem Solved

The stock ESPHome deep_sleep component has incomplete support for ESP32-C6 ext1 wakeup, causing compilation errors:
- `undefined reference to 'esphome::deep_sleep::DeepSleepComponent::set_ext1_wakeup'`

This occurs because the original ESPHome code incorrectly excludes ESP32-C6 from ext1 wakeup support, even though the ESP32-C6 hardware fully supports this feature according to ESP-IDF documentation.

## Key Changes

1. **Updated conditional compilation**: Removed ESP32-C6 from the exclusion list for ext1 wakeup
2. **Proper variant detection**: ESP32-C3 still doesn't support ext1 wakeup, but ESP32-C6 does
3. **Correct API usage**: ESP32-C6 uses the same ext1 wakeup API as other ESP32 variants

## Component Structure

- `__init__.py` - Python configuration and validation
- `deep_sleep_component.h` - C++ header with class definitions
- `deep_sleep_component.cpp` - Main component implementation
- `deep_sleep_esp32.cpp` - ESP32-specific implementation (including C6 support)
- `deep_sleep_esp8266.cpp` - ESP8266-specific implementation

## Usage

To use this external component in your ESPHome configuration:

```yaml
external_components:
  - source:
      type: local
      path: external_components

# Then use deep_sleep as normal
deep_sleep:
  id: deep_sleep_1
  sleep_duration: 12h
  run_duration: 90s
  esp32_ext1_wakeup:
    mode: ANY_HIGH
    pins:
      - GPIO2
      - GPIO4
      - GPIO5
      - GPIO6
```

## ESP32-C6 Ext1 Wakeup Details

The ESP32-C6 supports ext1 wakeup with the following features:
- Multiple GPIO pins can be configured as wakeup sources
- Two modes: ANY_HIGH (wake when any pin goes high) or ALL_LOW (wake when all pins go low)
- Pins 0-7 are supported for wakeup on ESP32-C6
- The wakeup status can be read using `esp_sleep_get_ext1_wakeup_status()`

## Compatibility

This component maintains full backward compatibility with all ESP32 variants while adding proper ESP32-C6 support.