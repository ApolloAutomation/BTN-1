# BTN-1 Zigbee branch parity

Two branches carry this firmware. They are intentionally identical except for the items below. Any firmware change must land on both branches in the same working session; this file is the contract that keeps drift from returning.

## The only allowed differences

| Item | z2m branch | ZHA branch |
| --- | --- | --- |
| `substitutions: model` in BTN-1.yaml | "BTN-1-Z2M" | "BTN-1-ZHA" |
| BTN-1.yaml header comment | Zigbee2MQTT wording | ZHA wording |
| apollo_btn1_quirk.py | absent | present |
| apollo_btn1.mjs | canonical copy | carried for reference |
| Readme.md / PAIRING_GUIDE.md | Z2M instructions | ZHA instructions |

Everything else, including Core.yaml (byte for byte), the endpoint map, partitions_zb.csv, and the component pins, must be identical.

## How to check

```
git diff z2m ZHA -- Integrations/Zigbee
```

The diff must touch only the files and hunks listed above. Run this before releasing either branch.

## How to apply a change to both branches

Commit the change on one branch, then cherry-pick the commit to the other and resolve only the allowed differences. The rework itself was applied this way (z2m commit cherry-picked onto ZHA, then the ZHA adaptation commit on top).
