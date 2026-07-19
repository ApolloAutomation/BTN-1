# Apollo BTN-1 Zigbee Pairing Guide (Zigbee2MQTT)

This guide covers the Zigbee2MQTT variant of the BTN-1 Zigbee firmware (model `BTN-1-Z2M`). If you use ZHA, flash the ZHA variant from the `ZHA` branch instead; its guide lives there.

## Prerequisites

- Home Assistant with Zigbee2MQTT 2.x and a working coordinator
- BTN-1 flashed with the firmware from this directory (see Readme.md)
- BTN-1 on USB-C power or a charged battery

## Step 1: Install the external converter

1. Copy `apollo_btn1.mjs` into the `external_converters` folder inside your Zigbee2MQTT data directory. Create the folder if it does not exist. The data directory is the one that contains Zigbee2MQTT's `configuration.yaml`.
2. On Zigbee2MQTT 2.11.0 and later, enable external converters in `configuration.yaml` (they are off by default there):

   ```yaml
   advanced:
     enable_external_js: true
   ```

3. Restart Zigbee2MQTT.
4. Check the Zigbee2MQTT log for a line confirming the external converter loaded.

Zigbee2MQTT 1.x cannot load this converter (it predates the ES module format); use Zigbee2MQTT 2.x.

## Step 2: Pair

1. In the Zigbee2MQTT frontend, enable permit join.
2. On the BTN-1, hold the side reset button for 5 to 10 seconds and release. The serial log shows "Zigbee reset, entering pairing mode" and the device reboots into pairing.
3. Wait for the interview to complete. The device appears with vendor "Apollo Automation", model "BTN-1".
4. When the device joins, the LEDs flash green and the buzzer chirps.

A freshly flashed device (after `erase_flash`) enters pairing mode on its own; the reset hold is only needed for re-pairing.

## Step 3: What you should see

- `action`: button events named `button_1_single`, `button_1_double`, `button_1_triple`, `button_1_hold`, and the same for buttons 2 to 4.
- `button_1_pressed` to `button_4_pressed`: raw pressed state, for automations that need press and release.
- `battery` and `voltage`: from the standard battery cluster.
- Four lights (`light_1` to `light_4`): on/off, brightness, and XY color per button.

## Example automation (Home Assistant)

Zigbee2MQTT publishes the button events as device triggers, so you can use them directly in a device automation, or trigger on MQTT state:

```yaml
automation:
  - alias: "BTN-1 button 1 toggles the living room light"
    trigger:
      - platform: device
        domain: mqtt
        device_id: <your BTN-1 device>
        type: action
        subtype: button_1_single
    action:
      - service: light.toggle
        target:
          entity_id: light.living_room
```

LED feedback works the same as any light. The exact entity id depends on the friendly name you give the device in Zigbee2MQTT; find it on the device page. With a device named `btn_1` it looks like:

```yaml
      - service: light.turn_on
        target:
          entity_id: light.btn_1_light_1
        data:
          rgb_color: [0, 255, 0]
          brightness: 128
```

## Reset button reference

| Gesture | Effect |
| --- | --- |
| Tap | Status LED: blue joined, yellow not joined |
| Hold 5 to 10 seconds, release | Zigbee reset, enter pairing mode |
| Hold 10 seconds | Full factory reset: ESPHome preferences and Zigbee network data |

## Troubleshooting

**Device not found during pairing**
1. Confirm permit join is enabled in Zigbee2MQTT.
2. Hold the reset button 5 to 10 seconds and release; watch for the pairing log line over serial.
3. Move the device within a couple of meters of the coordinator for the first join.
4. If it still refuses, erase the flash completely and reflash (Readme.md), which clears any stale network data.

**Device shows as unsupported**
- The converter is not loaded. Recheck Step 1 and the Zigbee2MQTT log, then re-pair or re-interview the device.

**Actions not arriving**
- Check the Zigbee2MQTT log for `genMultistateInput` reports when you press a button. If reports arrive but no action shows, the converter version does not match the firmware; update both from the same branch.

**Battery values missing**
- The battery cluster sits on endpoint 1 and reports at most every few minutes. Give it time after pairing, or trigger a read from the device page.

**Re-pairing after a coordinator change**
- Hold the reset button 5 to 10 seconds and release, then pair as above. The 10 second factory reset is not required for re-pairing.
