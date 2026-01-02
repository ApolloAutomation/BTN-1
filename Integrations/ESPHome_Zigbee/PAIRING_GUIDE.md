# Apollo BTN-1 ZHA Pairing Guide

This guide walks you through pairing your Apollo BTN-1 Zigbee device with Home Assistant using the ZHA (Zigbee Home Automation) integration.

## Prerequisites

Before you begin, ensure you have:

- [ ] Home Assistant installed and running
- [ ] ZHA integration configured with a compatible Zigbee coordinator
- [ ] BTN-1 flashed with Zigbee firmware (`btn1_zigbee_simple_FIXED.yaml`)
- [ ] BTN-1 charged or connected to power

## Supported Coordinators

The BTN-1 Zigbee firmware has been tested with:

- **Recommended**: SkyConnect (Silicon Labs)
- Sonoff ZBDongle-E (EFR32)
- Sonoff ZBDongle-P (CC2652)
- ConBee II
- HUSBZB-1

## Pairing Steps

### Step 1: Prepare the BTN-1

**For First-Time Flashing (IMPORTANT!):**
1. Erase the flash completely first to clear any stale Zigbee network data:
   ```bash
   esptool.py --chip esp32c6 erase_flash
   ```
2. Then flash the firmware:
   ```bash
   cd BTN-1/Integrations/ESPHome_Zigbee/examples
   esphome upload btn1_zigbee_simple_FIXED.yaml --device /dev/cu.usbmodem*
   ```
3. After flashing (with erased flash), the device should automatically enter pairing mode

**For Existing Device / Re-Pairing:**
1. Press and hold the **RESET button** (small button on the side, not the 4 main buttons) for **5-10 seconds**
2. Serial log will show: `*** ZIGBEE RESET - Entering pairing mode ***`
3. The device will leave any existing network and search for open networks

**Button Hold Durations:**
- **3-5 seconds**: Prevent deep sleep (keep device awake)
- **5-10 seconds**: Reset Zigbee (enter pairing mode) ⭐
- **10+ seconds**: Factory reset (erase all settings)

When in pairing mode, the BTN-1 serial log will show Zigbee scanning activity.

### Step 2: Enable ZHA Pairing Mode

1. Open Home Assistant
2. Go to **Settings** → **Devices & Services**
3. Click on **ZHA** (Zigbee Home Automation)
4. Click the **"Add Device"** button (+ icon)

![ZHA Add Device](https://via.placeholder.com/600x300/1a1a1a/ffffff?text=ZHA+Add+Device+Button)

### Step 3: Wait for Discovery

1. ZHA will scan for new devices
2. The BTN-1 should appear within 30-60 seconds
3. It will be identified as **"apollo-btn-1-zb"** or similar

### Step 4: Complete Pairing

1. Click on the discovered device
2. Optionally assign it to an area
3. Click **"Finish"**

### Step 5: Verify Entities

After pairing, the following entities should appear:

| Entity | Type | Description |
|--------|------|-------------|
| `binary_sensor.apollo_btn_1_zb_button_1` | Binary Sensor | Button 1 state |
| `binary_sensor.apollo_btn_1_zb_button_2` | Binary Sensor | Button 2 state |
| `binary_sensor.apollo_btn_1_zb_button_3` | Binary Sensor | Button 3 state |
| `binary_sensor.apollo_btn_1_zb_button_4` | Binary Sensor | Button 4 state |
| `sensor.apollo_btn_1_zb_battery_level` | Sensor | Battery % |
| `sensor.apollo_btn_1_zb_battery_voltage` | Sensor | Battery voltage |
| `light.apollo_btn_1_zb_btn_1_light` | Light | Button 1 LED |
| `light.apollo_btn_1_zb_btn_2_light` | Light | Button 2 LED |
| `light.apollo_btn_1_zb_btn_3_light` | Light | Button 3 LED |
| `light.apollo_btn_1_zb_btn_4_light` | Light | Button 4 LED |

## Creating Automations

Since the BTN-1 exposes binary sensors for button states, you can create automations in Home Assistant to handle button presses.

### Example: Button 1 Toggle Light

```yaml
automation:
  - alias: "BTN-1 Button 1 Toggle Living Room Light"
    trigger:
      - platform: state
        entity_id: binary_sensor.apollo_btn_1_zb_button_1
        to: "on"
    action:
      - service: light.toggle
        target:
          entity_id: light.living_room
```

### Example: Button with LED Feedback

```yaml
automation:
  - alias: "BTN-1 Button 2 with Feedback"
    trigger:
      - platform: state
        entity_id: binary_sensor.apollo_btn_1_zb_button_2
        to: "on"
    action:
      # Flash the button LED
      - service: light.turn_on
        target:
          entity_id: light.apollo_btn_1_zb_btn_2_light
        data:
          rgb_color: [0, 255, 0]
          brightness: 255
      # Perform action
      - service: switch.toggle
        target:
          entity_id: switch.bedroom_fan
      # Turn off LED after delay
      - delay:
          milliseconds: 500
      - service: light.turn_off
        target:
          entity_id: light.apollo_btn_1_zb_btn_2_light
```

### Example: Long Press Detection

Since double-click and hold events aren't supported over Zigbee, you can implement long-press detection using binary sensor duration:

```yaml
automation:
  - alias: "BTN-1 Button 3 Long Press"
    trigger:
      - platform: state
        entity_id: binary_sensor.apollo_btn_1_zb_button_3
        to: "on"
        for:
          seconds: 2
    action:
      - service: script.turn_on
        target:
          entity_id: script.long_press_action
```

## LED Customization

Each button has an individually controllable RGB LED. Use them for status indication:

### Set All LEDs to Same Color

```yaml
script:
  btn1_set_all_leds:
    sequence:
      - service: light.turn_on
        target:
          entity_id:
            - light.apollo_btn_1_zb_btn_1_light
            - light.apollo_btn_1_zb_btn_2_light
            - light.apollo_btn_1_zb_btn_3_light
            - light.apollo_btn_1_zb_btn_4_light
        data:
          rgb_color: [255, 100, 0]
          brightness: 128
```

### Status-Based LED Colors

```yaml
automation:
  - alias: "BTN-1 LED Status Indicator"
    trigger:
      - platform: state
        entity_id: sensor.apollo_btn_1_zb_battery_level
    action:
      - choose:
          - conditions:
              - condition: numeric_state
                entity_id: sensor.apollo_btn_1_zb_battery_level
                below: 20
            sequence:
              - service: light.turn_on
                target:
                  entity_id: light.apollo_btn_1_zb_btn_1_light
                data:
                  rgb_color: [255, 0, 0]  # Red for low battery
          - conditions:
              - condition: numeric_state
                entity_id: sensor.apollo_btn_1_zb_battery_level
                above: 80
            sequence:
              - service: light.turn_on
                target:
                  entity_id: light.apollo_btn_1_zb_btn_1_light
                data:
                  rgb_color: [0, 255, 0]  # Green for good battery
```

## Troubleshooting

### Device Not Found During Pairing

1. **Trigger Zigbee Reset**: Hold reset button **5-10 seconds** (not 10+, which is factory reset)
   - Serial log should show: `*** ZIGBEE RESET - Entering pairing mode ***`
2. **Erase Flash & Re-flash**: Stale Zigbee NVS data may prevent pairing
   ```bash
   esptool.py --chip esp32c6 erase_flash
   esphome upload btn1_zigbee_simple_FIXED.yaml --device /dev/cu.usbmodem*
   ```
3. **Check Serial Logs**: Connect via USB and watch for:
   - `*** ZIGBEE NETWORK JOINED ***` = Successfully paired
   - Zigbee stack debug messages (enable with `debug: true` in zigbee config)
4. **Move Closer**: Bring device within 2 meters of coordinator
5. **Check Coordinator**: Ensure ZHA is functioning (`Developer Tools` → `Services` → Call `zha.permit`)

### Entities Missing After Pairing

1. Wait 2-3 minutes for full discovery
2. Check ZHA device page for all clusters
3. Try removing and re-pairing the device
4. Restart Home Assistant

### Intermittent Connection

1. Add Zigbee router devices to extend mesh network
2. Check for WiFi interference on Zigbee channel
3. Ensure device isn't in deep sleep (disable Prevent Sleep)
4. Check battery level

### LEDs Not Responding

1. Verify device is awake (press a button first)
2. Check ZHA device page for light cluster
3. Try toggling from ZHA device page directly

### Button Presses Not Registering

1. Check binary sensor states in Developer Tools
2. Verify automation triggers are correct
3. Ensure device isn't in deep sleep
4. Try pressing firmly on buttons

## Re-Pairing After Coordinator Change

If you change your Zigbee coordinator:

1. Remove device from ZHA
2. Factory reset BTN-1 (10+ second reset button hold)
3. Add device again through ZHA

## Deep Sleep Considerations

The BTN-1 uses deep sleep to conserve battery. While in deep sleep:

- Device is **not reachable** over Zigbee
- Button presses wake the device
- After wake, device reports states and returns to sleep
- Set "Prevent Sleep" switch to ON to keep device awake for debugging

## Network Information

To view device's Zigbee network details:

1. Go to **Settings** → **Devices & Services** → **ZHA**
2. Click on the BTN-1 device
3. Click **"Zigbee Device Info"**

This shows:
- Network address
- IEEE address
- Device type
- LQI (Link Quality Indicator)
- RSSI (Signal Strength)
- Supported clusters

## Support

If you continue to have issues:

- Check ESPHome logs via serial: `esphome logs btn1_zigbee_simple_FIXED.yaml --device /dev/cu.usbmodem*`
- Join the Apollo Automation Discord: https://discord.gg/apolloautomation
- Open a GitHub issue: https://github.com/ApolloAutomation/BTN-1/issues
