# Apollo BTN-1 Zigbee Integration

This directory contains the ESPHome Zigbee configuration for the Apollo BTN-1 wireless button device.

## Overview

The BTN-1 is a 4-button wireless remote powered by an ESP32-C6 with native Zigbee support. This configuration allows the BTN-1 to pair with Home Assistant via ZHA (Zigbee Home Automation) integration.

## Hardware Specifications

| Component | Details |
|-----------|---------|
| MCU | ESP32-C6-MINI-1 |
| Flash | 8MB |
| Protocol | Zigbee 3.0 |
| Buttons | 4 tactile buttons |
| LEDs | 4x WS2812 RGB LEDs |
| Battery | LiPo with MAX17048 fuel gauge |
| Wake-up | Deep sleep with button wake |

### GPIO Pinout

| GPIO | Function |
|------|----------|
| GPIO0 | I2C SCL |
| GPIO1 | I2C SDA |
| GPIO2 | Button 1 (wake-capable) |
| GPIO4 | Button 2 (wake-capable) |
| GPIO5 | Button 4 (wake-capable) |
| GPIO6 | Button 3 (wake-capable) |
| GPIO9 | Reset Button |
| GPIO18 | Accessory Power |
| GPIO19 | Power Latch |
| GPIO21 | WS2812 RGB LEDs |
| GPIO23 | Buzzer |

## Files

```
ESPHome_Zigbee/
├── README.md                         # This file
├── PAIRING_GUIDE.md                  # ZHA pairing instructions
├── Core_Hardware_Zigbee.yaml         # Hardware definitions (package)
├── partitions/
│   └── partitions_zb.csv             # Zigbee partition table
└── examples/
    ├── btn1_zigbee_simple.yaml       # Basic config using packages
    └── btn1_zigbee_simple_FIXED.yaml # Complete standalone config
```

## Quick Start

### Prerequisites

- ESPHome CLI installed (`pip install esphome`)
- USB-C cable for flashing
- Home Assistant with ZHA integration configured

### Compile and Flash

```bash
cd BTN-1/Integrations/ESPHome_Zigbee/examples

# Compile the firmware
esphome compile btn1_zigbee_simple_FIXED.yaml

# Flash via USB (device will be in boot mode)
esphome upload btn1_zigbee_simple_FIXED.yaml --device /dev/cu.usbmodem*
```

### Pairing with ZHA

See [PAIRING_GUIDE.md](PAIRING_GUIDE.md) for detailed instructions.

## Configuration Details

### Zigbee Component

This configuration uses the [zigbee_esphome](https://github.com/luar123/zigbee_esphome) external component by luar123 which enables native Zigbee support on ESP32-C6.

```yaml
external_components:
  - source: github://luar123/zigbee_esphome
    components: [zigbee]

zigbee:
  id: zigbee_device
  endpoints:
    - endpoint_id: 1
      device_type: ON_OFF_LIGHT
      components:
        - all
```

### Exposed Entities

When paired with ZHA, the following entities are auto-discovered:

| Entity Type | Name | Description |
|------------|------|-------------|
| Binary Sensor | Button 1 | Physical button state |
| Binary Sensor | Button 2 | Physical button state |
| Binary Sensor | Button 3 | Physical button state |
| Binary Sensor | Button 4 | Physical button state |
| Sensor | Battery Level | Battery percentage |
| Sensor | Battery Voltage | Battery voltage |
| Light | BTN 1 Light | RGB LED for button 1 |
| Light | BTN 2 Light | RGB LED for button 2 |
| Light | BTN 3 Light | RGB LED for button 3 |
| Light | BTN 4 Light | RGB LED for button 4 |
| Number | Sleep Duration | Deep sleep time in hours |
| Switch | Prevent Sleep | Disable deep sleep |

### Important Notes

#### What Works
- ✅ Binary sensors for button states
- ✅ Battery monitoring (voltage and percentage)
- ✅ RGB LED control
- ✅ Deep sleep with button wake-up
- ✅ Auto-discovery in ZHA

#### What Doesn't Work with Zigbee
- ❌ Event entities (click, double-click, hold) - not supported by Zigbee auto-discovery
- ❌ Complex automations on-device - must be done in Home Assistant

#### Internal Components (Not Exposed)
- Power Latch (GPIO19) - must stay on for device to work
- Accessory Power (GPIO18) - power control
- Reset Button (GPIO9) - factory reset only

## Partition Table

The custom partition table (`partitions_zb.csv`) allocates space for:
- NVS storage
- OTA partitions (dual app slots)
- SPIFFS
- Zigbee storage (zb_storage)
- Zigbee factory test config (zb_fct)
- Core dump storage

## Troubleshooting

### Device Won't Pair

1. **Factory Reset**: Hold the reset button (GPIO9) for 10+ seconds
2. **Check Coordinator**: Ensure ZHA coordinator is in pairing mode
3. **Distance**: Move device closer to coordinator during initial pairing
4. **Re-flash**: Try re-flashing the firmware

### LEDs Not Working

1. Check that `power_latch` switch is ON (should be ALWAYS_ON by default)
2. Verify GPIO21 connection to LED strip
3. Check LED color order (GRB for WS2812)

### Battery Reading Incorrect

1. Verify I2C bus connectivity (GPIO0, GPIO1)
2. Check MAX17048 chip is properly powered
3. Ensure battery is connected

### Deep Sleep Issues

1. Turn off "Prevent Sleep" switch
2. Verify wake-up pins are configured correctly
3. Check for any running processes preventing sleep

## Development

### Building from Source

```bash
# Clone the repo
git clone https://github.com/ApolloAutomation/BTN-1

# Navigate to Zigbee examples
cd BTN-1/Integrations/ESPHome_Zigbee/examples

# Compile
esphome compile btn1_zigbee_simple_FIXED.yaml
```

### Modifying the Configuration

The `Core_Hardware_Zigbee.yaml` file can be used as a package for custom configurations. Create a new YAML file and include it:

```yaml
packages:
  hardware: !include Core_Hardware_Zigbee.yaml

# Add your customizations here
```

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 25.10.10.2-zigbee | 2025-10-10 | Initial Zigbee release |

## License

This project is part of Apollo Automation's open-source hardware initiative.

## Support

- GitHub Issues: https://github.com/ApolloAutomation/BTN-1/issues
- Discord: https://discord.gg/apolloautomation
- Website: https://apolloautomation.com
