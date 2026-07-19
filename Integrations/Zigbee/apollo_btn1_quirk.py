"""ZHA quirk for the Apollo Automation BTN-1 (Zigbee firmware model BTN-1-ZHA).

Installation:
1. In Home Assistant, point ZHA at a custom quirks folder by adding this to
   configuration.yaml:

     zha:
       custom_quirks_path: /config/custom_zha_quirks/

2. Copy this file into that folder and restart Home Assistant.
3. Pair the BTN-1, or remove and re-pair it if it was already joined.

What this quirk adds on top of ZHA's native discovery:
- Device automation triggers for every button action (single, double,
  triple, hold on buttons 1 to 4), so the actions appear in the Home
  Assistant automation editor as device triggers.
- Named binary sensors for the raw pressed state of each button, replacing
  the generic entities ZHA would otherwise create for those clusters.

The lights on endpoints 5 to 8 and the battery on endpoint 1 are standard
clusters that ZHA handles without a quirk.

Firmware endpoint map (fixed):
  1-4: Buttons 1-4. BinaryInput presentValue is the raw pressed state,
       MultistateInput presentValue is the action value
       (1 single, 2 double, 3 triple, 4 hold).
       Endpoint 1 also carries PowerConfiguration (battery).
  5-8: BTN 1-4 lights (OnOff, LevelControl, ColorControl XY).
"""

from zigpy.quirks import CustomCluster
from zigpy.zcl.clusters.general import BinaryInput, MultistateInput

try:
    from zhaquirks.builder import EntityType, QuirkBuilder
except ImportError:
    # Older Home Assistant releases ship QuirkBuilder under zigpy
    from zigpy.quirks.v2 import EntityType, QuirkBuilder
from zhaquirks.const import (
    COMMAND,
    DOUBLE_PRESS,
    ENDPOINT_ID,
    LONG_PRESS,
    SHORT_PRESS,
    TRIPLE_PRESS,
    ZHA_SEND_EVENT,
)

ACTIONS = {1: "single", 2: "double", 3: "triple", 4: "hold"}

PRESS_TYPES = {
    "single": SHORT_PRESS,
    "double": DOUBLE_PRESS,
    "triple": TRIPLE_PRESS,
    "hold": LONG_PRESS,
}


class ApolloButtonEvents(CustomCluster, MultistateInput):
    """MultistateInput cluster that raises ZHA events for button actions."""

    def _update_attribute(self, attrid, value):
        super()._update_attribute(attrid, value)
        if attrid == 0x0055:
            action = ACTIONS.get(int(value))
            if action:
                self.listener_event(
                    ZHA_SEND_EVENT,
                    f"button_{self.endpoint.endpoint_id}_{action}",
                    {
                        "button": self.endpoint.endpoint_id,
                        "press_type": action,
                    },
                )


_TRIGGERS = {}
for _ep in (1, 2, 3, 4):
    for _action, _press_type in PRESS_TYPES.items():
        _TRIGGERS[(_press_type, f"button_{_ep}")] = {
            COMMAND: f"button_{_ep}_{_action}",
            ENDPOINT_ID: _ep,
        }

_builder = QuirkBuilder("Apollo Automation", "BTN-1-ZHA")

for _ep in (1, 2, 3, 4):
    _builder = _builder.replaces(
        ApolloButtonEvents,
        endpoint_id=_ep,
    ).prevent_default_entity_creation(
        endpoint_id=_ep,
        cluster_id=BinaryInput.cluster_id,
    ).binary_sensor(
        "present_value",
        BinaryInput.cluster_id,
        endpoint_id=_ep,
        entity_type=EntityType.STANDARD,
        fallback_name=f"Button {_ep} pressed",
        unique_id_suffix=f"button_{_ep}_pressed",
    )

_builder.device_automation_triggers(_TRIGGERS).add_to_registry()
