# Apollo BTN-1 Zigbee Pairing Guide (ZHA)

This guide covers the ZHA variant of the BTN-1 Zigbee firmware (model `BTN-1-ZHA`). If you use Zigbee2MQTT, flash the Zigbee2MQTT variant from the `z2m` branch instead; its guide lives there.

## Prerequisites

- Home Assistant with the ZHA integration and a working coordinator
- BTN-1 flashed with the firmware from this directory (see Readme.md)
- BTN-1 on USB-C power or a charged battery

## Step 1: Install the quirk

The device pairs and works without the quirk: lights, battery, and the raw button state clusters are standard. The quirk adds the button actions (single, double, triple, hold) as device triggers in the automation editor and gives the pressed-state sensors friendly names, so installing it is recommended.

1. Add a custom quirks folder to your Home Assistant `configuration.yaml`:

   ```yaml
   zha:
     custom_quirks_path: /config/custom_zha_quirks/
   ```

2. Copy `apollo_btn1_quirk.py` into that folder.
3. Restart Home Assistant.

## Step 2: Pair

1. In Home Assistant, go to Settings, then Devices and Services, open ZHA, and choose Add Device.
2. On the BTN-1, hold the side reset button for 5 to 10 seconds and release. The serial log shows "Zigbee reset, entering pairing mode" and the device reboots into pairing.
3. The device appears as manufacturer "Apollo Automation", model "BTN-1-ZHA". Pick an area and finish.
4. When the device joins, the LEDs flash green and the buzzer chirps.

A freshly flashed device (after `erase_flash`) enters pairing mode on its own; the reset hold is only needed for re-pairing.

## Step 3: What you should see

With the quirk installed:

- Device triggers for all sixteen button actions in the automation editor (buttons 1 to 4, each with single, double, triple, and hold).
- Binary sensors "Button 1 pressed" to "Button 4 pressed" for the raw state.
- Battery percentage and voltage sensors.
- Four lights, one per button, with on/off, brightness, and XY color.

Without the quirk, the lights and battery still work, the raw button states appear as generic binary sensors, and the action values appear as generic multistate sensors instead of triggers.

## Example automation

```yaml
automation:
  - alias: "BTN-1 button 1 toggles the living room light"
    trigger:
      - platform: device
        domain: zha
        device_id: <your BTN-1 device>
        type: remote_button_short_press
        subtype: button_1
    action:
      - service: light.toggle
        target:
          entity_id: light.living_room
```

You can also listen for the raw events on the `zha_event` bus; the command field carries names like `button_2_double`.

## Reset button reference

| Gesture | Effect |
| --- | --- |
| Tap | Status LED: blue joined, yellow not joined |
| Hold 5 to 10 seconds, release | Zigbee reset, enter pairing mode |
| Hold 10 seconds | Full factory reset: ESPHome preferences and Zigbee network data |

## Troubleshooting

**Device not found during pairing**
1. Confirm ZHA is in Add Device mode.
2. Hold the reset button 5 to 10 seconds and release; watch for the pairing log line over serial.
3. Move the device within a couple of meters of the coordinator for the first join.
4. If it still refuses, erase the flash completely and reflash (Readme.md), which clears any stale network data.

**No device triggers in the automation editor**
- The quirk is not loading. Check the Home Assistant log for quirk errors, confirm the `custom_quirks_path` points at the folder holding `apollo_btn1_quirk.py`, then remove and re-pair the device.

**Actions not firing**
- Watch `zha_event` in Developer Tools, then press a button. If nothing arrives, re-pair the device; if events arrive but automations do not run, the trigger subtype does not match, so re-select it in the editor.

**Battery values missing**
- The battery cluster sits on endpoint 1 and reports at most every few minutes. Give it time after pairing.

**Re-pairing after a coordinator change**
- Hold the reset button 5 to 10 seconds and release, then pair as above. The 10 second factory reset is not required for re-pairing.
