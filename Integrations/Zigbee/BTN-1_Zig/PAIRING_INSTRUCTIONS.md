# BTN-1 Zigbee Device - Home Assistant ZHA Pairing Instructions

## Overview

The BTN-1 is a 4-button Zigbee macro pad that can be paired with Home Assistant using the ZHA (Zigbee Home Automation) integration. The device supports pairing mode, factory reset, and provides visual LED feedback during all operations.

## Prerequisites

Before pairing your BTN-1 device:

1. **Home Assistant Setup**
   - Ensure Home Assistant is installed and running
   - ZHA integration must be installed and configured
   - A Zigbee coordinator (e.g., Sonoff Zigbee 3.0 USB Dongle, ConBee II) must be connected and recognized by ZHA

2. **BTN-1 Device**
   - Device must be powered on
   - Device firmware must be flashed (see flashing instructions)
   - Device should not be currently paired to another network

## Pairing Process

### Method 1: Initial Pairing (Device Not Previously Paired)

1. **Prepare Home Assistant**
   - Navigate to Settings → Devices & Services → ZHA
   - Click the "Configure" button
   - Select "Add Device" or enable "Permit Join"
   - Home Assistant will now search for devices for 60 seconds

2. **Enter Pairing Mode on BTN-1**
   - **Press and hold Button 1 for 10 seconds**
   - LED Indicators during pairing:
     - Fast blink (100ms on/off): Device searching for network
     - Slow blink (500ms on/off): Device joining network
     - Solid LED for 3 seconds: Successfully connected
     - Double blink pattern: Pairing failed

3. **Complete Pairing in Home Assistant**
   - The device should appear as "BTN-1" or "Apollo BTN-1" in the discovered devices
   - Click on the device to configure it
   - Assign it to a room/area if desired
   - The device will appear with 4 endpoints (one for each button)

### Method 2: Re-pairing After Factory Reset

If the device was previously paired and needs to be re-paired:

1. **Perform Factory Reset**
   - **Press and hold all 4 buttons simultaneously for 5 seconds**
   - LED will flash rapidly (20 flashes) to confirm factory reset
   - Device will restart automatically after 3 seconds

2. **Follow Method 1** to pair the device with Home Assistant

## LED Status Indicators

| LED Pattern | Duration | Meaning |
|------------|----------|---------|
| Fast Blink | 100ms on/off | Searching for Zigbee network |
| Slow Blink | 500ms on/off | Joining Zigbee network |
| Solid On | 3 seconds | Successfully joined network |
| Double Blink | 3 repetitions | Error/Failed to join |
| Rapid Flash | 20 flashes | Factory reset in progress |
| Quick Flash | 50ms | Button press acknowledged |
| Pulsing | 200ms intervals | Button being held |
| Triple Flash | Once | 10-second threshold reached |

## Button Functions After Pairing

Once paired with Home Assistant, each button provides:

| Button Action | Function | Default Behavior |
|--------------|----------|------------------|
| Short Press | Toggle | Send ON/OFF toggle command |
| Long Press (0.5s) | Dimming | Cycle through 25%, 50%, 75%, 100% brightness |
| Double Press | Scene | Recall associated scene |
| Button 1 Hold (10s) | Pairing | Enter pairing mode (for re-pairing) |
| All 4 Buttons (5s) | Reset | Factory reset device |

## Troubleshooting

### Device Not Discovered

1. **Check ZHA is in pairing mode**
   - Ensure "Permit Join" is enabled in ZHA
   - Try extending the pairing timeout in ZHA settings

2. **Verify button press**
   - Hold Button 1 for full 10 seconds
   - Watch for LED feedback confirming pairing mode

3. **Check distance**
   - Move device closer to Zigbee coordinator
   - Ensure no major obstacles between device and coordinator

4. **Try factory reset**
   - Hold all 4 buttons for 5 seconds
   - Wait for device restart
   - Retry pairing process

### Device Shows as Unknown

If the device pairs but shows as "Unknown manufacturer" or similar:

1. The device is still functional - this is a cosmetic issue
2. You can rename the device in Home Assistant
3. All 4 button endpoints should still be available

### Device Pairs but Buttons Don't Work

1. **Check entity availability**
   - Go to Developer Tools → States
   - Search for entities containing "btn"
   - Verify 4 switch entities exist

2. **Check Zigbee bindings**
   - In ZHA device page, click "Manage Zigbee Device"
   - Verify endpoints 1-4 are present
   - Check clusters are properly configured

3. **Power cycle the device**
   - Remove power from BTN-1
   - Wait 10 seconds
   - Restore power
   - Device should automatically reconnect

## Home Assistant Configuration

### Basic Automation Example

```yaml
automation:
  - alias: "BTN-1 Button 1 Toggle Lights"
    trigger:
      - platform: device
        device_id: YOUR_DEVICE_ID
        domain: zha
        type: remote_button_short_press
        subtype: button_1
    action:
      - service: light.toggle
        target:
          entity_id: light.living_room

  - alias: "BTN-1 Button 2 Dim Lights"
    trigger:
      - platform: device
        device_id: YOUR_DEVICE_ID
        domain: zha
        type: remote_button_long_press
        subtype: button_2
    action:
      - service: light.turn_on
        target:
          entity_id: light.bedroom
        data:
          brightness_step: 64  # 25% increments
```

### Device Information

- **Manufacturer**: Apollo Automation
- **Model**: BTN-1
- **Device Type**: On/Off Switch (HA profile)
- **Zigbee Version**: 3.0
- **Power Source**: Battery/USB
- **Endpoints**: 4 (one per button)
- **Clusters**: 
  - Basic (0x0000)
  - Identify (0x0003)
  - On/Off (0x0006)
  - Level Control (0x0008)
  - Scenes (0x0005)
  - Groups (0x0004)

## Advanced Features

### Group Control

Buttons can be assigned to Zigbee groups for controlling multiple devices:

1. Create a group in ZHA
2. Add target devices to the group
3. Bind BTN-1 button to the group
4. Button will now control all devices in the group

### Direct Binding

For faster response, buttons can be directly bound to Zigbee devices:

1. In ZHA, go to BTN-1 device page
2. Click "Manage Zigbee Device"
3. Select "Bindings" tab
4. Choose button endpoint and target device
5. Create binding

### OTA Updates

The BTN-1 supports Over-The-Air (OTA) firmware updates through ZHA:

1. Place firmware file in Home Assistant config/zigpy_ota/
2. Device will automatically check for updates
3. Updates can be triggered manually through ZHA interface

## Network Diagnostics

To verify successful pairing:

1. **Check signal strength**
   - ZHA device page shows LQI (Link Quality Indicator)
   - Values above 200 indicate good signal
   - Values below 100 may cause issues

2. **Monitor device logs**
   - Enable debug logging for zha component
   - Check for button press events in logs
   - Verify commands are being sent/received

3. **Test each button**
   - Create temporary automations
   - Trigger each button action
   - Verify all 4 endpoints respond

## Support

For additional support:
- Check device logs via serial console
- Verify firmware version is latest
- Contact Apollo Automation support
- Join Home Assistant community forums

## Safety Notes

- Do not attempt pairing while device firmware is updating
- Ensure stable power during factory reset
- Keep device firmware updated for best compatibility
- Document your button assignments for easy reconfiguration