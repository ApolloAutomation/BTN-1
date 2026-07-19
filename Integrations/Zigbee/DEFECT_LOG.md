# BTN-1 Zigbee defect ledger

51 defects were identified in the 26.1 era Zigbee firmware, its converter, and its docs during the July 2026 audit (line-by-line read of the firmware, the component at both the shipped and pinned commits, generated code, and both doc files, with adversarial cross-checks against the generated build). Disposition of every item in the 26.7.19.1-zigbee rework:

| ID | Summary | Disposition |
| --- | --- | --- |
| F1-1 | YAML declared router, component silently built end device | Fixed: role goes through the component (`router: false`), manual sdkconfig flags removed |
| F1-2 | End device advertised mains power, never sleepy | Fixed: `power_supply: 3`; true sleepy end device blocked on newer ESPHome, in remaining work |
| F1-3 | Prevent Sleep default masked all sleep bugs; unguarded sleep entry | Fixed by removing the deep sleep machinery entirely |
| F1-4 | ext1 wake relied on internal pulldowns that may float in sleep | Moot (no deep sleep); button pulldowns noted in hardware checklist item A |
| F1-5 | Wake-from-sleep button press never delivered | Moot (no deep sleep; device always listening) |
| F1-6 | Factory reset erased ESPHome NVS but not Zigbee credentials | Fixed: 10 s tier erases preferences then calls the Zigbee factory reset, one reboot |
| F1-7 | Dual timers made the 5 s and 10 s boundaries nondeterministic | Fixed: single on_multi_click with exclusive tiers |
| F1-8 | Preference write lost across zigbee.reset; 90 s pairing window race | Fixed: no prevent-sleep preference exists; pairing window is now unlimited |
| F1-9 | zigbee.report transmitted every attribute on every press and release | Fixed: per-attribute force reports only |
| F1-10 | Shipped binary ran automations on the Zigbee task (races, crash risk) | Fixed by the component pin (thread-safe event queue) |
| F1-11 | Sleep Duration 0 caused a reboot loop; knob unreachable over Zigbee | Fixed by removing the number and deep sleep |
| F1-12 | Battery filter passed NaN and negatives; battery on a generic analog cluster | Fixed: NaN/range clamp; standard PowerConfiguration cluster |
| F1-13 | MAX17048 driver: no probe, no mark_failed, skipped SOC read on error | Open: component-level improvement, listed upstream; probe behavior in hardware checklist |
| F1-14 | Power latch rails float in deep sleep, unreconciled with wake design | Moot (no deep sleep); latch reboot behavior in hardware checklist item G |
| F1-15 | zigbee.reset takes the ZB lock unbounded on the main thread | Open: upstream issue listed in ZIGBEE_NOTES; gesture unchanged |
| F1-16 | %d for uint32, redundant prevent_sleep call before reset | Fixed: the lambda no longer exists |
| F2-1..F2-5 | Partition table: top 4 MB unallocated, 60 KB gap, unused coredump/spiffs, OTA slots without transport | Documented as deliberate: offsets left byte-identical so reflashing without erase preserves Zigbee NVRAM; slots reserved for possible future Zigbee OTA |
| F2-6 | Component partition validation accepts a half-valid table | Open: upstream issue; our table is fully valid |
| F2-7 | Converter endpoint map wrong on every entry | Fixed: converter rewritten against the frozen map |
| F2-8 | Converter battery on a cluster the firmware lacked | Fixed: firmware now exposes genPowerCfg; converter uses fz.battery |
| F2-9 | Action names could never match (button_8..11) | Fixed: multistate action model, names button_N_single/double/triple/hold |
| F2-10 | configure() crashed on current Zigbee2MQTT (stale logger signature) | Fixed: modern signature, no logger parameter |
| F2-11 | Color-temp converters for an XY-only light | Fixed: XY-only modernExtend light |
| F2-12 | multiEndpointSkip listed a property that does not exist | Fixed |
| F2-13 | Z2M 1.x install instructions | Fixed: Z2M 2.x directory install documented, 1.x noted |
| F2-14 | Buzzer and rtttl dead code | Fixed: join chirp and feedback use it |
| F2-15 | cycleCounter global unused | Fixed: removed |
| F2-16 | Five copies of unreachable light effects | Fixed: removed |
| F2-17 | ZHA-centric comments on the Z2M reference | Fixed: per-branch headers |
| F2-18 | Unnamed factory_reset switch implying an exposure that never existed | Fixed: switch removed; wipe is a direct preferences reset in the 10 s tier |
| F2-19 | Per-press INFO logs ahead of the radio call; %d format | Fixed: hot-path logs removed |
| F2-20 | All three substitutions unused; stale version | Fixed: substitutions wired, version 26.7.19.1-zigbee |
| F2-21 | Wrong-case filename in flash instructions; wrong on-air name claim | Fixed in the new headers and docs |
| F2-22 | Reset tier comment misstated the real windows | Fixed: tiers now match their documentation |
| F2-23..F2-35 | Docs: nonexistent paths and files, invalid config snippet, impossible entity tables, inverted troubleshooting advice, wrong factory reset claims, placeholder image, stale HA/Z2M UI references | Fixed: PAIRING_GUIDE.md and Readme.md rewritten from the actual code on both branches |
