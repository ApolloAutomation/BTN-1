# BTN-1 Zigbee firmware notes

Covers both Zigbee branches. Firmware version 26.7.19.1-zigbee, built with ESPHome 2026.3.1 and zigbee_esphome pinned to 98b54b83ab080628e661b8a28b8b03b6c3bc38bd (v1.x, ZBOSS stack). Everything here is compile-verified and code-read only; nothing in this revision has run on hardware yet. HARDWARE_VERIFICATION.md is the checklist for that.

## Which branch for which stack

| Branch | Model on the air | Integration glue |
| --- | --- | --- |
| `z2m` | BTN-1-Z2M | apollo_btn1.mjs external converter |
| `ZHA` | BTN-1-ZHA | apollo_btn1_quirk.py custom quirk |

The firmware is otherwise identical (see BRANCH_PARITY.md). Both are Zigbee end devices, mains-or-battery powered, radio always on.

## Identity and endpoint map

Manufacturer "Apollo Automation", model per branch, ApplicationVersion 1, DateCode 20260719, PowerSource battery (3).

| EP | Device type | Clusters and attributes |
| --- | --- | --- |
| 1 | CUSTOM_ATTR | BinaryInput: presentValue (raw Button 1 state, force-report), description "Button 1". MultistateInput: presentValue (action value, force-report), numberOfStates 5, description "Button 1 Action". PowerConfiguration: BatteryVoltage 0x0020 (100 mV units, battery volts x10), BatteryPercentageRemaining 0x0021 (half percent, percent x2), both reporting |
| 2-4 | CUSTOM_ATTR | Same BinaryInput + MultistateInput shape for Buttons 2-4 |
| 5-8 | COLOR_DIMMABLE_LIGHT | OnOff, LevelControl (currentLevel), ColorControl (currentX, currentY, colorCapabilities=8: XY only) for BTN 1-4 Light |

Action values on MultistateInput presentValue: 1 single, 2 double, 3 triple, 4 hold. Timing windows match the Wi-Fi firmware: click is a press up to 300 ms followed by 0.5 s quiet; double and triple chain such presses with gaps up to 0.5 s; hold is a press of 1 to 5 seconds. A press held longer than 5 seconds produces no action on purpose (same as the Wi-Fi firmware); the raw BinaryInput state still shows it.

## Reporting behavior

- Button state and action attributes report through explicit per-attribute report actions in the automations, with the component's reporting engine left unregistered for them. This sends exactly one report per edge and avoids the duplicate-event window that combining engine reporting with immediate reports would open. Action reports are additionally gated on being joined, so presses before the first join cannot queue and replay as phantom events at join time.
- Battery attributes report through the standard reporting engine. The MAX17048 is polled every 5 minutes, so expect battery updates at that cadence. Coordinators can reconfigure reporting with standard ZCL; the configuration persists in Zigbee NVRAM.
- Component-level defaults at this pin are min interval 10 s, max interval 0, delta 0, and are not adjustable from YAML. ZHA additionally clamps some minimum intervals to 30 s.

## Join, reset, and persistence

- Fresh (factory-new) boot: the device starts network steering, retries 10 times at 1 s, then every 600 s. There is no pairing timeout: the device stays awake and keeps trying.
- Joined state, keys, and bindings persist in the zb_storage partition; a normal reboot rejoins from NVRAM without re-steering. The green join flash and chirp fire on fresh joins.
- Reset button tiers (single automation, no overlapping timers): tap shows join status on the LEDs (blue joined, yellow not); a 5 to 10 second hold released performs a Zigbee reset (erases Zigbee NVRAM, reboots into pairing); a 10 second hold performs a full factory reset (ESPHome preferences erased first, then Zigbee NVRAM, one reboot). Unlike the previous firmware, the 10 second tier really does erase the Zigbee network data.
- Removing the device from the coordinator sends a network Leave with rejoin=false; the stack erases its NVRAM and reopens joining.

## What changed for existing flashers

A unit flashed with the previous binary (model "BTN-1", endpoints: lights 1-4, switch 5, analog battery 6-7, buttons 8-11) must be reflashed over USB (erase flash first) and re-paired. Entities are re-created; the old converter file must be removed from Zigbee2MQTT. The old converter never matched that firmware's real endpoint layout, so no working Z2M setup existed to break.

## Removed relative to the previous Zigbee firmware, with reasons

- Deep sleep, Sleep Duration, Prevent Sleep: the component has no sleepy end device support at any ESPHome 2026.3.x compatible commit. The previous configuration combined a 24 h sleep cycle with an always-on end device role and a default that prevented sleep entirely; the honest capability is an always-on device. Battery drains in days when not on USB-C; measure in hardware verification.
- ESP Reboot button, cycleCounter global, light effects: dead over Zigbee (no transport can reach them).
- AnalogInput battery endpoints: replaced by the standard PowerConfiguration cluster.

## Removed relative to the Wi-Fi firmware, with reasons

- Remote buzzer control (the api play_buzzer action): there is no Zigbee transport for arbitrary tone playback. The buzzer now serves local feedback only (join chirp). If a Zigbee path is ever wanted, a writable custom attribute could carry a tone index.
- ESP die temperature, uptime, and the other diagnostic sensors: deliberate scope cut to keep the endpoint count at eight on a battery device; PUMP-1 demonstrates the temperature pattern if it is ever wanted here.

## Local feedback and coordinator state

The status LED gestures drive the physical strip directly. The tap status indicator uses only the Button 1 light so coordinator-set colors on the other three survive; the join flash uses the whole strip (lights are expected dark at join time). The component connects attributes to lights one way (Zigbee to light), so a locally driven flash is not reported back; a coordinator that had set a color sees its own state until it next reads or commands. This is a component limitation, noted for the hardware pass.

## Known limitations

- No Zigbee OTA: firmware updates are USB serial only.
- Reporting intervals are compile-time fixed (coordinator can reconfigure).
- Without the quirk (ZHA) or converter (Z2M), buttons appear as generic BinaryInput/MultistateInput entities.
- The component's event queue holds 32 events; extreme bursts log a warning and drop. Four buttons mashed simultaneously stay well under this.
- Manual force-reports go to the coordinator (address 0x0000, endpoint 1) and ignore bindings.
- Groups and scenes clusters exist on the light endpoints via SDK defaults but are untested upstream; scenes are not implemented by the component.
- Identify is a no-op: the component attaches an Identify cluster to every endpoint but registers no handler, so an Identify command blinks nothing. Upstream lists identify-driven light effects as planned work.

## Component pin notes and upstream issues worth filing

Pinned commit 98b54b8 is the newest v1.x commit compatible with ESPHome 2026.3.x (newer ones need 2026.4+ APIs). Relative to the c90bd47 the old binary shipped with, it fixes Zigbee-task callbacks running ESPHome automations unsynchronized (crash and race risk), runs on_join on the main thread, and fixes a crash on Off commands to endpoints without LevelControl. Issues observed in the component at this pin, worth reporting upstream:

1. setAttr's attribute lookup (find_attr) raises KeyError when it scans an attribute without an id. Worked around here by giving every attribute on the button endpoints an id.
2. ZigBeeComponent::reset() takes the Zigbee lock with an unbounded wait on the main thread.
3. The partition validation accepts a table containing only one of zb_storage and zb_fct.
4. The time cluster sync check has an operator precedence bug (sync_status & 0x3 != 0), so a coordinator advertising only "Synchronized" never updates the clock.
5. The binding-table readback allocates device params and never frees or uses them.

## Change summary for the wiki

The BTN-1 Zigbee firmware was rebuilt around a fixed endpoint map. Buttons now deliver single, double, and triple clicks plus hold as Zigbee events with the same timing behavior as the Wi-Fi firmware, battery reporting moved to the standard Zigbee battery cluster, and each variant identifies itself by stack (BTN-1-ZHA or BTN-1-Z2M) so the matching quirk or converter applies cleanly. The reset button gained a real factory reset that clears Zigbee network data, and the pairing gesture is a 5 to 10 second hold. Deep sleep was removed: the previous firmware advertised it but shipped with it disabled, and the Zigbee component cannot yet run as a sleepy end device, so USB-C power is the intended mode. External component versions are pinned for reproducible builds. This firmware is available to flash for advanced users; it is not the main BTN-1 firmware.
