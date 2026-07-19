# BTN-1 Core.yaml backport log

The Zigbee Core.yaml was forked from the ESPHome Core.yaml at version 25.10.10.2 (October 2025) and never updated while main advanced to 26.3.2.1. This log records what the 26.7.19.1-zigbee rework brought across from main and what it deliberately did not.

## Backported

1. Board spec modernisation (from fc6edd0): the `board: esp32-c6-devkitm-1` key and dio flash-mode option are gone; ESPHome resolves the board from the variant.
2. Reset button rework (pattern from 4000519): the on_press/on_release timestamp lambdas became declarative on_multi_click tiers. The Zigbee-specific 5 to 10 second pairing tier was preserved (main has no such tier), and the tiers no longer overlap or race.
3. Button debounce (from the click-timing lineage): `delayed_off: 10ms` on all four buttons.
4. Multi-click timing windows (wholesale import of main's final state): click, double, triple, and hold definitions with main's exact timings, re-targeted from ESPHome event entities to Zigbee MultistateInput action values.
5. Battery entity name casing ("Battery voltage" / "Battery level") to match main.
6. Version housekeeping: version substitution bumped to 26.7.19.1-zigbee and actually wired into the project block.

## Deliberately not backported, with reasons

- Everything Wi-Fi, API, OTA, web server, captive portal, improv, version sensors, IP/RSSI/Online sensors, OTA Mode: the Zigbee firmware has no transport for any of it.
- The api on_client_connected wake/report/sleep flow and wake-button text sensor: deep sleep is removed from the Zigbee firmware, and there is no API.
- The factory-test flash script (runTest/testCycleCount/testScript) and its on_boot hook: factory-line feature; port it later if Zigbee-flashed units go through the same test fixture. The standalone green-light-on-boot fix only exists inside this feature, so it travels with it or not at all.
- The deep_sleep external-component removal lineage: the fork already used the built-in component; nothing to do.
- main's `&common_light` anchor refactor: the fork keeps explicit per-light ids because the Zigbee endpoints reference them.
- main's prevent_sleep/ota_mode gating: both entities no longer exist here.

## Divergences that must survive future backports

See BRANCH_PARITY.md for branch rules and ZIGBEE_NOTES.md for the design. In particular: buttons stay named and exported through explicit endpoints (never internal-with-events like main), no event: entities, no wifi/api blocks, platform config stays in BTN-1.yaml rather than Core.yaml, and the reset button must keep its 5 to 10 second Zigbee reset tier.
