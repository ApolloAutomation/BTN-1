# Apollo BTN-1 Zigbee Firmware (Zigbee2MQTT variant)

This directory contains an alternative Zigbee firmware for the Apollo BTN-1. It is not Apollo's main firmware for this product. The BTN-1 ships with ESPHome Wi-Fi firmware; the ESP32-C6 gives it Wi-Fi, Bluetooth LE, and Thread on the same chip, and Zigbee is also possible at the chip level for advanced users. This firmware is exactly that advanced case.

This branch carries the Zigbee2MQTT variant (model `BTN-1-Z2M`), which pairs with Zigbee2MQTT using the external converter `apollo_btn1.mjs` in this directory. A ZHA variant (model `BTN-1-ZHA`) lives on the `ZHA` branch.

## What you get

- Four buttons with on-device click detection: single, double, and triple click plus hold, with the same timing windows as the Wi-Fi firmware, delivered as action events. Raw press and release state is also available.
- Four individually controllable RGB backlights (XY color and brightness).
- Battery voltage and percentage through a standard Zigbee battery cluster.
- Local LED and buzzer feedback for joining, pairing, and status.

## What you give up compared to the Wi-Fi firmware

- No over-the-air updates. Firmware changes require a USB-C cable.
- No deep sleep. This firmware runs as an always-on Zigbee end device, because the Zigbee component in use has no sleepy end device support. USB-C power is the intended mode. On battery alone the radio stays on and the charge lasts days, not months.
- No web interface, no Home Assistant native API, and no version or diagnostic sensors. Link quality comes from your coordinator instead.

## Hardware

| Component | Details |
| --- | --- |
| MCU | ESP32-C6-MINI-1, 8MB flash |
| Buttons | 4 Cherry MX compatible hot swap, plus a side reset button |
| LEDs | 4 WS2812 RGB, one per button |
| Battery | LiPo with MAX17048 fuel gauge |

| GPIO | Function |
| --- | --- |
| GPIO0 | I2C SCL (MAX17048) |
| GPIO1 | I2C SDA (MAX17048) |
| GPIO2 | Button 1 |
| GPIO4 | Button 2 |
| GPIO6 | Button 3 |
| GPIO5 | Button 4 |
| GPIO9 | Reset button |
| GPIO18 | Accessory power rail |
| GPIO19 | Power latch |
| GPIO21 | WS2812 RGB LEDs |
| GPIO23 | Buzzer |

## Files

```
Zigbee/
  Readme.md                  This file
  PAIRING_GUIDE.md           Pairing walkthrough for Zigbee2MQTT
  BTN-1.yaml                 Firmware entry point (platform, Zigbee endpoints)
  Core.yaml                  Hardware package (buttons, LEDs, battery, feedback)
  apollo_btn1.mjs            Zigbee2MQTT external converter
  apollo_btn1_quirk.py       ZHA quirk (for the ZHA variant on the ZHA branch)
  partitions_zb.csv          Partition table with Zigbee NVRAM regions
  ZIGBEE_NOTES.md            Endpoint map, reporting, and design notes
  HARDWARE_VERIFICATION.md   Ordered on-device test checklist
  BRANCH_PARITY.md           Allowed differences between the two branches
  BACKPORT_LOG.md            What came across from the Wi-Fi Core.yaml
  DEFECT_LOG.md              Audit findings and their dispositions
  ZIGBEE_BEFORE_*.txt        Signature records, pre-rework
  ZIGBEE_AFTER_*.txt         Signature records, post-rework
```

There is no prebuilt binary: the earlier release predated this rework and its converter could not work, so it was removed. Build from source below.

## Building and flashing

Requirements: ESPHome 2026.3.x and a USB-C cable.

```
cd BTN-1/Integrations/Zigbee

# First flash over Wi-Fi firmware, or when switching Zigbee firmware versions:
# erase the flash first so stale Zigbee network data cannot interfere.
esptool.py --chip esp32c6 erase_flash

esphome compile BTN-1.yaml
esphome upload BTN-1.yaml --device /dev/cu.usbmodem*
```

The external components are pinned to exact commits inside `BTN-1.yaml`, so builds are reproducible.

## Zigbee endpoint map

| Endpoint | Function | Clusters |
| --- | --- | --- |
| 1 | Button 1, battery | BinaryInput (pressed state), MultistateInput (action), PowerConfiguration |
| 2 | Button 2 | BinaryInput, MultistateInput |
| 3 | Button 3 | BinaryInput, MultistateInput |
| 4 | Button 4 | BinaryInput, MultistateInput |
| 5 | BTN 1 Light | OnOff, LevelControl, ColorControl (XY) |
| 6 | BTN 2 Light | OnOff, LevelControl, ColorControl (XY) |
| 7 | BTN 3 Light | OnOff, LevelControl, ColorControl (XY) |
| 8 | BTN 4 Light | OnOff, LevelControl, ColorControl (XY) |

MultistateInput action values: 1 single, 2 double, 3 triple, 4 hold. A hold is a press of 1 to 5 seconds. Identity on the air: manufacturer "Apollo Automation", model "BTN-1-Z2M".

## Reset button

| Gesture | Effect |
| --- | --- |
| Tap | Status LED: blue means joined to a Zigbee network, yellow means not joined |
| Hold 5 to 10 seconds, release | Zigbee reset: leaves the network, erases Zigbee network data, reboots into pairing mode |
| Hold 10 seconds | Factory reset: erases ESPHome preferences and Zigbee network data, then reboots |

## Support

- Pairing help: see PAIRING_GUIDE.md
- GitHub issues: https://github.com/ApolloAutomation/BTN-1/issues
- Discord: https://discord.gg/apolloautomation
