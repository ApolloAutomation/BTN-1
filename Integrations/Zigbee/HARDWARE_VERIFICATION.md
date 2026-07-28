# BTN-1 Zigbee hardware verification checklist

Nothing in firmware 26.7.19.1-zigbee has run on a device. Compilation and code reading are the only verification so far. Work through this list in order on both branches; items marked (both) behave identically on either branch.

## A. Flash and boot (both)

1. `esptool.py --chip esp32c6 erase_flash`, then flash the branch build. Pass: device boots, serial log shows Zigbee steering starting. Fail suggests: partition table or component regression.
2. Watch one full minute of log. Pass: no I2C error spam from the MAX17048 (a warning every 5 minutes at most). Repeated read failures suggest the fuel gauge rail is not up; note which rail powers the MAX17048 (schematic question left open by the audit: battery direct versus the GPIO18 accessory rail).

## B. Pairing (per branch)

3. ZHA branch: install the quirk, pair. Pass: device appears as "Apollo Automation BTN-1-ZHA"; sixteen device triggers listed in the automation editor; battery percentage and voltage sensors populate within 10 minutes; four lights work; "Button N pressed" binary sensors follow presses. Quirk not matching suggests a QuirkBuilder API drift in the installed ZHA; check the HA log.
4. z2m branch: install the converter, pair. Pass: vendor Apollo Automation, model BTN-1; action events named button_N_single/double/triple/hold arrive; lights l1 to l4 control the four LEDs; battery populates. configure() errors in the Z2M log suggest a herdsman API drift.
5. Join feedback: green LED flash and chirp on the fresh join (both).

## C. Buttons and events (both)

6. Single, double, triple, hold on each button. Pass: exactly one action per gesture, correct button number, and the raw pressed state toggles. Compare click feel against a Wi-Fi BTN-1: the timing windows are the same values.
7. Press held longer than 5 seconds: no action fires (dead zone by design), raw state shows the long press. Confirm the docs describe reality.
8. Burst test: mash all four buttons rapidly for several seconds. Pass: no reboot, no watchdog, at worst a dropped-event warning in the log. This exercises the thread-safety fixes the pin was chosen for.

## D. Battery (both)

9. On battery, confirm voltage (x10, 100 mV units) and percentage (x2, half percent) arrive with sane values in HA. A percentage above 100 or below 0 must not appear (firmware clamps).
10. Battery life: measure average current with the radio idle-joined and LEDs off. Publish the expected runtime; the always-on end device configuration is the honest mode, and users should know the number.
11. Unplug the battery (USB powered): sensors should stop updating with periodic warnings, no crash, no Zigbee impact.

## E. Reset tiers (both)

12. Tap: blue LED when joined, yellow when not.
13. Hold 5 to 10 s, release: device leaves, reboots into pairing, old coordinator entry goes stale. Pass: it does NOT silently rejoin the old network.
14. Hold 10 s: after reboot the device must be factory new BOTH ways: not joined to the old network (Zigbee NVRAM cleared) and ESPHome preferences (light states) back to defaults. This is the fix for the old firmware's factory reset that left Zigbee credentials in place; verify it explicitly.
15. Remove the device from the coordinator UI. Pass: device wipes and reopens joining (leave-with-reset path).

## F. Lights (both)

16. XY color fidelity: set pure red, green, blue, white from HA on each button light. Note any hue shift (the XY to RGB conversion happens on-device).
17. Local feedback overlap: trigger the status LED (tap) while a light is on. The strip is shared; confirm the light state recovers or document that feedback temporarily overrides the partitions.
18. State echo: change a light from HA and confirm the coordinator state matches afterward. Local script changes (join flash) are not pushed back to the coordinator at this pin (one-way connect); confirm impact is acceptable.

## G. Power path (both)

19. Confirm the device stays powered through a reboot on battery (power latch GPIO19 is ALWAYS_ON early in boot; the old deep-sleep latch questions are moot now, but a reboot must not cut power).
20. Accessory rail GPIO18: with a side or top attachment fitted, confirm the rail is up and the attachment behaves. Attachment data integration over Zigbee does not exist yet (remaining work).

## H. Regression comparison (informational)

21. Pair the old released binary (releases/apollo-btn-1-zigbee.bin) on a spare unit and confirm the failure modes the audit predicted (converter dead, battery absent in Z2M, factory reset leaving credentials). Useful for support conversations with existing flashers; skip if no spare unit.

## Bench session results, 2026-07-28 (sleepy WIP build)

Passed: flash and boot, fresh-join interview with all nine endpoints, silent NVRAM rejoin after reflash, raw button press and release in Home Assistant, all four lights with color set and readback, command delivery at 3 s keep-alive (3 to 5 s latency, zero losses, ordered burst delivery), battery percentage and voltage including live charge tracking and engine-driven reports, factory reset and Zigbee reset tiers, remote attribute reads, ZHA reconfigure without error, Identify confirmed as harmless no-op.

Found and fixed during the session: stuck button states from the component's split set/report lock race (now atomic force reports); command delivery misses at 5 s and 10 s keep-alive against the ZBT-2 (parent buffer window; pinned at 3 s pending a coordinator-timeout measurement).

Lessons recorded: ZHA caches endpoint layouts, so firmware endpoint changes need remove-and-re-pair, not reset; a nearby network with permit join open will steal devices during commissioning (one join went to an unidentified foreign network before succeeding here); the USB console disappears in light sleep, so bench logging requires a non-sleepy diagnostic build.

Remaining, in priority order: storage mode entry, wake press, and post-wake rejoin; click actions end to end with the quirk installed; duplicate-action observation window; idle current in normal and storage modes (meter); multi-day battery drain; Z2M variant of the sleepy work; coordinator indirect-timeout measurement before any keep-alive increase.
