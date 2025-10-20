# BTN-1 Zigbee Firmware

## Table of Contents
- [Project Overview](#project-overview)
- [Building the Firmware](#building-the-firmware)
- [Device Features](#device-features)
- [Home Assistant Integration](#home-assistant-integration)
- [Configuration Options](#configuration-options)
- [Troubleshooting Guide](#troubleshooting-guide)
- [API Reference](#api-reference)

---

## Project Overview

The BTN-1 is a battery-powered, ESP32-C6-based Zigbee macro pad featuring 4 programmable buttons designed for smart home automation. Built with power efficiency in mind, it delivers 16-20 months of battery life while providing reliable wireless control through the Zigbee protocol.

### Key Features
- **4 Programmable Buttons** with multiple press actions (short, long, double)
- **Zigbee 3.0 Compatibility** using ESP32-C6's native 802.15.4 radio
- **Battery Optimized** with advanced power management and light sleep
- **Home Assistant Ready** with native support for both ZHA and Zigbee2MQTT
- **Direct Device Binding** for standalone operation without a hub
- **Group Control** for managing multiple devices simultaneously
- **Scene Support** for one-touch ambiance control
- **OTA Updates** for wireless firmware upgrades
- **Customizable** button mappings and behaviors

### Hardware Requirements
- **MCU**: ESP32-C6-DevKitC-1 or compatible board
- **Power**: 
  - Battery-powered: 2xAA or CR2032 (depending on variant)
  - USB-powered during development
- **Buttons**: 4x momentary push buttons
- **GPIO Connections**:
  - Button 1: GPIO 9
  - Button 2: GPIO 10
  - Button 3: GPIO 11
  - Button 4: GPIO 3
- **Optional**: Battery voltage monitoring via ADC

---

## Building the Firmware

### Prerequisites

1. **PlatformIO** (Recommended)
   ```bash
   # Install via pip
   pip install platformio
   
   # Or via system package manager
   brew install platformio  # macOS
   ```

2. **ESP-IDF v5.3.2** (Automatically handled by PlatformIO)
   - Framework is specified in platformio.ini
   - No manual installation required

3. **USB Drivers**
   - CP2102/CH340 drivers for your development board
   - Available from board manufacturer

### Build Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/ApolloAutomation/BTN-1.git
   cd BTN-1/Integrations/Zigbee/BTN-1_Zig
   ```

2. **Build the Firmware**
   ```bash
   # Using PlatformIO
   pio run
   
   # Or build specific environment
   pio run -e esp32-c6-devkitc-1
   ```

3. **Build Output**
   - Firmware binary: `.pio/build/esp32-c6-devkitc-1/firmware.bin`
   - Partition table: `.pio/build/esp32-c6-devkitc-1/partitions.bin`
   - Bootloader: `.pio/build/esp32-c6-devkitc-1/bootloader.bin`

### Flash Instructions

1. **Connect Device**
   - Connect ESP32-C6 board via USB
   - Hold BOOT button if manual download mode required

2. **Flash Firmware**
   ```bash
   # Flash everything (recommended for first time)
   pio run -t upload
   
   # Or manually specify port
   pio run -t upload --upload-port /dev/ttyUSB0
   ```

3. **Monitor Serial Output**
   ```bash
   pio device monitor -b 115200
   ```

### Partition Table Details

The device uses a custom partition scheme optimized for Zigbee and OTA updates:

```
# Name       Type    SubType   Offset      Size       Purpose
nvs          data    nvs       0x9000      0x6000     Non-volatile storage
otadata      data    ota       0xf000      0x2000     OTA metadata
phy_init     data    phy       0x11000     0x1000     PHY calibration data
ota_0        app     ota_0     0x20000     0x300000   Primary app partition (3MB)
ota_1        app     ota_1     0x320000    0x300000   Secondary app partition (3MB)
zb_storage   data    fat       0x620000    0x4000     Zigbee persistent data
zb_fct       data    fat       0x624000    0x400      Zigbee factory data
```

---

## Device Features

### Button Functionality

Each of the 4 buttons supports three distinct press types:

| Press Type | Duration | Action | Default Behavior |
|------------|----------|--------|------------------|
| **Short Press** | < 500ms | Toggle | Send ON/OFF toggle command |
| **Long Press** | > 1s | Dimming | Cycle brightness: 25% → 50% → 75% → 100% → 25% |
| **Double Press** | 2x within 500ms | Scene | Recall scene (Button N recalls Scene N) |

### Zigbee Network Details

- **Device Type**: On/Off Switch (0x0103)
- **Profile**: Home Automation (0x0104)
- **Endpoints**: 4 (one per button)
- **Default Channel**: 20 (configurable, supports 11-26)
- **Network Role**: End Device (battery optimized)
- **Supported Clusters**:
  - Basic (0x0000)
  - Identify (0x0003)
  - Groups (0x0004)
  - Scenes (0x0005)
  - On/Off (0x0006)
  - Level Control (0x0008)
  - On/Off Switch Configuration (0x0007)

### Power Consumption Specifications

The device implements aggressive power management for extended battery life:

- **Active Current**: ~15-20mA (during transmission)
- **Sleep Current**: <50µA (light sleep mode)
- **Battery Life**: 16-20 months (with 100 button presses/day)
- **Sleep Strategy**:
  - Immediate sleep after button release
  - Wake on button press (GPIO interrupt)
  - Periodic wake for Zigbee keep-alive (3s default)
  - Adaptive sleep based on usage patterns

### Battery Life Expectations

Based on typical usage patterns:
- **Light Use** (20 presses/day): ~20 months
- **Normal Use** (100 presses/day): ~18 months
- **Heavy Use** (300 presses/day): ~16 months

*Note: Battery life varies with battery type, temperature, and network conditions*

---

## Home Assistant Integration

### ZHA (Zigbee Home Automation) Integration

#### Pairing Instructions

1. **Enable Pairing Mode in Home Assistant**
   - Navigate to: Settings → Devices & Services → ZHA
   - Click "Configure" → "Add Device"
   - ZHA enters pairing mode (60 second timeout)

2. **Activate Device Pairing**
   - Power on the BTN-1 (insert batteries or connect USB)
   - If previously paired: Hold Button 4 for 10+ seconds to factory reset
   - Device automatically enters pairing mode on first boot

3. **Complete Pairing**
   - Device appears as "BTN-1" or "Apollo Automation BTN-1"
   - Interview completes automatically
   - 4 button entities created

#### Entity Creation and Naming

ZHA creates entities following this pattern:
```
button.btn1_button_1  # Endpoint 1
button.btn1_button_2  # Endpoint 2
button.btn1_button_3  # Endpoint 3
button.btn1_button_4  # Endpoint 4
```

Customize entity names in Home Assistant UI:
1. Click on device → Click on entity
2. Click settings icon → Change name
3. Suggested names: "Living Room Remote Button 1", etc.

#### ZHA Automation Examples

**Simple Toggle Light**
```yaml
automation:
  - alias: "BTN-1 Button 1 Toggle Light"
    trigger:
      - platform: event
        event_type: zha_event
        event_data:
          device_ieee: "00:17:88:01:08:xx:xx:xx"  # Your device IEEE
          endpoint_id: 1
          command: "toggle"
    action:
      - service: light.toggle
        target:
          entity_id: light.living_room
```

**Brightness Control (Long Press)**
```yaml
automation:
  - alias: "BTN-1 Button 2 Brightness"
    trigger:
      - platform: event
        event_type: zha_event
        event_data:
          device_ieee: "00:17:88:01:08:xx:xx:xx"
          endpoint_id: 2
          cluster_id: 8
          command: "move_to_level"
    action:
      - service: light.turn_on
        target:
          entity_id: light.bedroom
        data:
          brightness: "{{ trigger.event.data.args[0] }}"
```

**Scene Control (Double Press)**
```yaml
automation:
  - alias: "BTN-1 Button 3 Movie Scene"
    trigger:
      - platform: event
        event_type: zha_event
        event_data:
          device_ieee: "00:17:88:01:08:xx:xx:xx"
          endpoint_id: 3
          cluster_id: 5
          command: "recall_scene"
    action:
      - service: scene.turn_on
        target:
          entity_id: scene.movie_time
```

### Zigbee2MQTT Integration

#### Pairing Instructions

1. **Enable Permit Join**
   ```bash
   # Via MQTT
   mosquitto_pub -t zigbee2mqtt/bridge/request/permit_join -m '{"value": true}'
   
   # Or via Web UI
   # Click "Permit join" button in Z2M frontend
   ```

2. **Pair Device**
   - Power on BTN-1 or factory reset if needed
   - Device appears in devices list within 60 seconds
   - Rename device from default IEEE address

3. **Configuration**
   ```yaml
   # In zigbee2mqtt configuration.yaml
   devices:
     '0x0017880108xxxxxx':
       friendly_name: 'btn1_living_room'
       retain: true
   ```

#### Zigbee2MQTT Automation Examples

**Button Actions via MQTT**
```yaml
automation:
  - alias: "Z2M BTN-1 Light Control"
    trigger:
      - platform: mqtt
        topic: "zigbee2mqtt/btn1_living_room"
        value_template: "{{ value_json.action }}"
    action:
      - choose:
          - conditions:
              - condition: template
                value_template: "{{ trigger.payload_json.action == 'button_1_single' }}"
            sequence:
              - service: light.toggle
                entity_id: light.living_room
          - conditions:
              - condition: template
                value_template: "{{ trigger.payload_json.action == 'button_1_double' }}"
            sequence:
              - service: scene.turn_on
                entity_id: scene.bright
```

#### Binding Configuration

**Direct Binding (Works without Home Assistant)**

ZHA Method:
```yaml
service: zha.bind
data:
  source_device_ieee: "00:17:88:01:08:xx:xx:xx"
  target_device_ieee: "00:17:88:01:09:yy:yy:yy"
  source_endpoint: 1  # Button number
  target_endpoint: 1  # Light endpoint
  cluster_id: 6       # On/Off cluster
```

Zigbee2MQTT Method:
```bash
mosquitto_pub -t zigbee2mqtt/bridge/request/device/bind \
  -m '{"from": "btn1_living_room", "to": "light_1", 
       "clusters": ["genOnOff"], "endpoint": 1}'
```

---

## Configuration Options

### Channel Selection

The device supports Zigbee channels 11-26. Default is channel 20.

**Changing Channel (Requires Rebuild)**
```c
// In zigbee_device.h
#define BTN1_ZIGBEE_PRIMARY_CHANNEL_MASK  (1 << 20)  // Single channel
// Or for multi-channel scan:
#define BTN1_ZIGBEE_PRIMARY_CHANNEL_MASK  ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK
```

### Sleep Parameters

Configure power management behavior:

```c
// In main.c - Modify before power_management_init()
power_config_t pm_config = {
    .activity_timeout_ms = 5000,        // Time before sleep (default: 5s)
    .min_sleep_duration_ms = 100,       // Minimum sleep time
    .adaptive_sleep_enabled = true,     // Smart sleep based on usage
    .battery_monitoring_enabled = true  // Enable battery ADC
};
```

### GPIO Pin Assignments

Default button GPIO mapping:
```c
// In button_handler.c
static const uint8_t button_pins[BUTTON_COUNT] = {
    GPIO_NUM_9,   // Button 1
    GPIO_NUM_10,  // Button 2
    GPIO_NUM_11,  // Button 3
    GPIO_NUM_3    // Button 4
};
```

To change GPIO assignments, modify the array above and rebuild.

### NVS Stored Settings

The following settings persist across reboots:
- Device bindings (up to 4 per button)
- Dimming levels for each button
- Scene IDs for each button
- Group assignments
- Network security keys

**Factory Reset** (Clear all settings):
- Hold Button 4 for 10+ seconds
- Or send factory reset command via coordinator

### Button Behavior Customization

```c
// In button_handler.h - Timing configuration
#define BUTTON_SHORT_PRESS_MAX_MS    500    // Max duration for short press
#define BUTTON_LONG_PRESS_MIN_MS     1000   // Min duration for long press
#define BUTTON_DOUBLE_PRESS_MAX_MS   500    // Max time between presses
#define BUTTON_DEBOUNCE_MS           50     // Debounce time
```

---

## Troubleshooting Guide

### Common Issues and Solutions

#### Device Won't Pair

**Symptoms**: Not discovered during pairing, times out

**Solutions**:
1. **Verify Channel Configuration**
   - ZHA default: Channel 15
   - Z2M default: Channel 11
   - Device scans all channels but may take longer

2. **Factory Reset**
   ```
   Hold Button 4 for 10+ seconds
   - LED flashes rapidly (if equipped)
   - Serial output: "Factory reset initiated"
   ```

3. **Check Interference**
   - Move away from Wi-Fi routers
   - Use USB extension cable for coordinator
   - Avoid USB 3.0 ports (2.4GHz interference)

#### Buttons Not Responding

**Check Network Status**:
```bash
# Monitor serial output
pio device monitor -b 115200

# Look for:
# "Zigbee stack started"
# "Device joined network"
```

**Common Causes**:
- Low battery (check voltage)
- Out of range (LQI < 100)
- Parent router offline
- Binding misconfiguration

#### Intermittent Response

**Diagnosis Steps**:
1. Check Link Quality (LQI)
   - ZHA: Device page → Zigbee info
   - Z2M: Device page → Link quality

2. Add Zigbee Router
   - Place router between device and coordinator
   - Improves range and reliability

3. Check for Interference
   - Change Zigbee channel
   - Scan Wi-Fi channels for overlap

### Debug Logging

#### Enable Serial Debug Output
```c
// In main.c or via menuconfig
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
```

#### Home Assistant Debug Logging
```yaml
# configuration.yaml
logger:
  default: info
  logs:
    homeassistant.components.zha: debug
    zigpy: debug
    zigpy_cc: debug  # For CC2652 coordinators
    bellows: debug   # For EmberZNet coordinators
```

#### Zigbee2MQTT Debug
```yaml
# zigbee2mqtt configuration.yaml
advanced:
  log_level: debug
  log_to_console: true
```

### Network Rejoining

If device loses connection:

1. **Automatic Rejoin**
   - Device attempts rejoin every 30 seconds
   - Uses stored network key
   - No intervention needed

2. **Manual Rejoin**
   - Short press all 4 buttons simultaneously
   - Triggers immediate rejoin attempt

3. **Force New Join**
   - Factory reset (Button 4 hold)
   - Re-pair as new device

### Factory Reset Procedure

**Method 1: Button Combination**
1. Hold Button 4 for 10+ seconds
2. All settings cleared
3. Device enters pairing mode

**Method 2: Serial Command**
```bash
# Connect to serial console
pio device monitor -b 115200
# Type:
factory_reset
```

**Method 3: Via Coordinator**
- ZHA: Device page → "Remove Device" → "Factory Reset"
- Z2M: Device page → "Force Remove"

**What Gets Reset**:
- ✓ Network credentials
- ✓ All bindings
- ✓ Button configurations
- ✓ Group memberships
- ✓ Custom scene IDs
- ✗ Firmware (remains unchanged)

---

## API Reference

### Main Modules

#### `main.c` - Application Entry Point
Core initialization and Zigbee task management.
```c
// Key functions:
void app_main(void)           // Entry point, initializes all subsystems
esp_zb_task(void *pvParameters)  // Main Zigbee protocol task
```

#### `button_handler.c` - Button Input Processing
Handles GPIO interrupts and button event detection.
```c
// Key APIs:
esp_err_t button_handler_init(void)
esp_err_t button_register_callback(button_event_callback_t callback)

// Event types:
typedef enum {
    BUTTON_EVENT_SHORT_PRESS,
    BUTTON_EVENT_LONG_PRESS,
    BUTTON_EVENT_DOUBLE_PRESS,
} button_event_type_t;
```

#### `zigbee_device.c` - Zigbee Protocol Implementation
Manages Zigbee endpoints, clusters, and command handling.
```c
// Device configuration:
void btn1_zigbee_init(void)
void btn1_button_event_handler(button_event_t *event)

// Command handlers:
esp_err_t btn1_send_toggle_cmd(uint8_t endpoint)
esp_err_t btn1_send_level_cmd(uint8_t endpoint, uint8_t level)
esp_err_t btn1_send_scene_cmd(uint8_t endpoint, uint8_t scene_id)
```

#### `power_management.c` - Power Optimization
Controls sleep modes and battery monitoring.
```c
// Power management APIs:
esp_err_t power_management_init(const power_config_t *config)
esp_err_t power_enter_sleep(void)
esp_err_t power_activity_notify(void)
esp_err_t power_get_battery_percentage(uint8_t *percentage)
```

### Configuration Constants

#### Network Configuration
```c
#define ESP_ZB_ZED_CONFIG()                                \
{                                                          \
    .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,                  \
    .install_code_policy = INSTALLCODE_POLICY_ENABLE,      \
    .nwk_cfg.zed_cfg = {                                   \
        .ed_timeout = ESP_ZB_ED_AGING_TIMEOUT_64MIN,       \
        .keep_alive = 3000,  /* 3 second keep-alive */     \
    }                                                      \
}
```

#### Device Identity
```c
#define MANUFACTURER_NAME     "Apollo Automation"
#define MODEL_IDENTIFIER      "BTN-1"
#define BTN1_DEVICE_VERSION   1
#define BTN1_ENDPOINT_COUNT   4
```

#### Cluster Configuration
```c
// Supported clusters per endpoint
Input Clusters:
  - Basic (0x0000)
  - Identify (0x0003)  
  - On/Off Switch Config (0x0007)

Output Clusters:
  - Groups (0x0004)
  - Scenes (0x0005)
  - On/Off (0x0006)
  - Level Control (0x0008)
```

### Customization Points

#### Adding Custom Commands
```c
// In zigbee_device.c
void btn1_button_event_handler(button_event_t *event) {
    switch (event->event) {
        case BUTTON_EVENT_CUSTOM:
            // Add your custom command here
            send_custom_cluster_cmd(event->button_num);
            break;
    }
}
```

#### Modifying Button Behavior
```c
// Define custom press patterns in button_handler.h
#define BUTTON_TRIPLE_PRESS_MAX_MS   1000

// Add detection logic in button_handler.c
if (press_count == 3 && time_diff < BUTTON_TRIPLE_PRESS_MAX_MS) {
    event.event = BUTTON_EVENT_TRIPLE_PRESS;
}
```

#### Custom Power Profiles
```c
// Create usage-specific power configurations
const power_config_t bedroom_config = {
    .activity_timeout_ms = 10000,      // Longer timeout for bedroom
    .adaptive_sleep_enabled = false,   // Consistent behavior
};

const power_config_t kitchen_config = {
    .activity_timeout_ms = 2000,       // Quick sleep for kitchen
    .adaptive_sleep_enabled = true,    // Learn usage patterns
};
```

---

## Additional Resources

- **Test Plan**: See [test_plan.md](../test_plan.md) for comprehensive testing procedures
- **Support**: [Apollo Automation Community](https://community.apolloautomation.com)
- **Updates**: Check for firmware updates in Home Assistant or via OTA
- **Issues**: Report bugs on [GitHub Issues](https://github.com/ApolloAutomation/BTN-1/issues)

## License

Copyright (c) 2025 Apollo Automation. All rights reserved.

This firmware is proprietary software. Distribution and use are subject to the Apollo Automation license agreement.