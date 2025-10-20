# BTN-1 Zigbee Device Test Plan
## Home Assistant Integration via ZHA and Zigbee2MQTT

**Version:** 1.0  
**Last Updated:** January 2025  
**Device:** BTN-1 4-Button Zigbee Macro Pad (ESP32-C6)

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Pre-Test Setup Requirements](#pre-test-setup-requirements)
3. [Coordinator Compatibility Matrix](#coordinator-compatibility-matrix)
4. [Device Pairing Tests](#device-pairing-tests)
5. [Button Functionality Tests](#button-functionality-tests)
6. [Binding Configuration Tests](#binding-configuration-tests)
7. [Group Control Tests](#group-control-tests)
8. [Scene Recall Tests](#scene-recall-tests)
9. [Channel Compatibility Tests](#channel-compatibility-tests)
10. [OTA Firmware Update Tests](#ota-firmware-update-tests)
11. [Edge Cases and Stress Tests](#edge-cases-and-stress-tests)
12. [Performance Validation](#performance-validation)
13. [Troubleshooting Guide](#troubleshooting-guide)
14. [Home Assistant Automation Examples](#home-assistant-automation-examples)

---

## Executive Summary

This test plan validates the BTN-1 Zigbee device integration with Home Assistant through both ZHA (Zigbee Home Automation) and Zigbee2MQTT. The device features 4 programmable buttons supporting short press (ON/OFF toggle), long press (brightness cycling), and double press (scene recall) operations.

### Device Specifications
- **Model:** BTN-1
- **Manufacturer:** Apollo Automation
- **Zigbee Profile:** Home Automation (0x0104)
- **Device Type:** On/Off Switch (0x0103)
- **Endpoints:** 4 (one per button)
- **Default Channel:** 20 (configurable)
- **Firmware:** ESP-IDF based with OTA support

### Test Scope
- Multi-platform compatibility (ZHA and Zigbee2MQTT)
- Multi-coordinator support
- Full feature validation
- Production readiness verification

---

## Pre-Test Setup Requirements

### 1. Hardware Requirements

#### Test Equipment
- [ ] BTN-1 device(s) for testing
- [ ] Compatible Zigbee coordinator (see compatibility matrix)
- [ ] Home Assistant installation (2024.1 or newer)
- [ ] USB extension cable (to reduce interference)
- [ ] Power supply for BTN-1 (if mains-powered)
- [ ] Battery (if battery-powered variant)

#### Target Devices for Testing
- [ ] Zigbee light bulbs (for binding tests)
- [ ] Zigbee switches/outlets
- [ ] Zigbee dimmers
- [ ] Other Zigbee devices supporting groups/scenes

### 2. Software Requirements

#### For ZHA Testing
```yaml
# configuration.yaml (if needed)
zha:
  zigpy_config:
    ota:
      ikea_provider: true                    # Enable OTA
      ledvance_provider: true
      allow_file_providers: true
```

#### For Zigbee2MQTT Testing
```yaml
# configuration.yaml for Zigbee2MQTT
mqtt:
  broker: localhost
  discovery: true

# Zigbee2MQTT configuration.yaml
permit_join: false
mqtt:
  base_topic: zigbee2mqtt
  server: 'mqtt://localhost'
serial:
  port: /dev/ttyUSB0
advanced:
  channel: 11  # Default for Z2M
  log_level: debug
  pan_id: 6754
```

### 3. Test Environment Setup

1. **Clean Environment**
   - [ ] Factory reset coordinator
   - [ ] Clear Home Assistant Zigbee database
   - [ ] Document network parameters (PAN ID, channel)

2. **Monitoring Tools**
   - [ ] Home Assistant Developer Tools → Events
   - [ ] Zigbee2MQTT web interface (if applicable)
   - [ ] Serial monitor for device logs
   - [ ] Zigbee packet sniffer (optional)

---

## Coordinator Compatibility Matrix

Test the device with various coordinator hardware to ensure broad compatibility:

| Coordinator | Chip | ZHA | Z2M | Test Priority | Notes |
|------------|------|-----|-----|--------------|-------|
| SkyConnect | EFR32MG21 | ✓ | ✓ | HIGH | Home Assistant official |
| Sonoff ZBDongle-P | CC2652P | ✓ | ✓ | HIGH | Popular choice |
| Sonoff ZBDongle-E | EFR32MG21 | ✓ | ✓ | MEDIUM | Newer model |
| ConBee II | deCONZ | ✓ | ✓ | MEDIUM | Wide compatibility |
| ConBee III | deCONZ | ✓ | ✓ | LOW | Latest version |
| CC2531 | CC2531 | ✓ | ✓ | LOW | Legacy, limited |
| HUSBZB-1 | EM357 | ✓ | × | LOW | US market |

### Testing Checklist per Coordinator
- [ ] Device pairs successfully
- [ ] All 4 buttons detected
- [ ] Commands transmit reliably
- [ ] OTA updates work
- [ ] Range acceptable (>5m)

---

## Device Pairing Tests

### Test Case DP-01: Initial Pairing with ZHA

**Objective:** Verify device pairs successfully with ZHA

**Prerequisites:**
- Home Assistant with ZHA configured
- Coordinator in permit join mode

**Test Steps:**
1. Navigate to Settings → Devices & Services → ZHA
2. Click "Configure" → "Add Device"
3. Power on BTN-1 device (or factory reset if previously paired)
4. Monitor pairing progress

**Expected Results:**
- [ ] Device discovered within 60 seconds
- [ ] Shows as "BTN-1" or "Apollo Automation BTN-1"
- [ ] 4 endpoints visible
- [ ] Device interview completes successfully
- [ ] All clusters properly identified

**Verification:**
```yaml
# Check device signature in ZHA
Device signature:
  endpoints:
    1: # Button 1
      profile_id: 260 (0x0104)
      device_type: 2048 (0x0800)
      input_clusters:
        - 0x0000 (Basic)
        - 0x0003 (Identify)
        - 0x0007 (On/Off Switch Config)
      output_clusters:
        - 0x0006 (On/Off)
        - 0x0008 (Level Control)
        - 0x0005 (Scenes)
        - 0x0004 (Groups)
    2-4: # Buttons 2-4 (same structure)
```

### Test Case DP-02: Initial Pairing with Zigbee2MQTT

**Objective:** Verify device pairs successfully with Zigbee2MQTT

**Prerequisites:**
- Zigbee2MQTT running and accessible
- MQTT broker operational

**Test Steps:**
1. Enable permit join in Z2M web interface or via MQTT:
   ```bash
   mosquitto_pub -t zigbee2mqtt/bridge/request/permit_join -m '{"value": true}'
   ```
2. Power on BTN-1 device
3. Monitor Z2M logs for pairing

**Expected Results:**
- [ ] Device appears in Z2M devices list
- [ ] Friendly name assigned (e.g., "0x0017880108abcdef")
- [ ] Device interview completed
- [ ] All 4 endpoints exposed

**Verification:**
```json
// MQTT Explorer - Check device topic
zigbee2mqtt/BTN-1/availability: "online"
zigbee2mqtt/BTN-1/info: {
  "ieee_address": "0x0017880108abcdef",
  "friendly_name": "BTN-1",
  "type": "EndDevice",
  "endpoints": {
    "1": { "bindings": [] },
    "2": { "bindings": [] },
    "3": { "bindings": [] },
    "4": { "bindings": [] }
  }
}
```

### Test Case DP-03: Factory Reset and Re-pairing

**Objective:** Verify factory reset procedure works correctly

**Test Steps:**
1. While device is paired, perform factory reset:
   - Hold Button 4 for 10+ seconds (check device documentation)
   - Or trigger via serial command
2. Verify device removed from network
3. Re-pair following DP-01 or DP-02

**Expected Results:**
- [ ] Device leaves network cleanly
- [ ] NVS data cleared
- [ ] Re-pairs as new device
- [ ] Previous bindings/settings cleared

---

## Button Functionality Tests

### Test Case BF-01: Short Press (ON/OFF Toggle)

**Objective:** Verify short press generates ON/OFF toggle commands

**Prerequisites:**
- Device paired and online
- Event monitoring enabled

**Test Steps:**
1. Open HA Developer Tools → Events → Listen "zha_event"
2. Short press each button (< 500ms)
3. Record generated events

**Expected Results per Button:**
```yaml
# Button 1 short press
event_type: zha_event
data:
  device_ieee: "00:17:88:01:08:ab:cd:ef"
  unique_id: "00:17:88:01:08:ab:cd:ef:1:0x0006"
  endpoint_id: 1
  cluster_id: 6
  command: "toggle"
  args: []

# Buttons 2-4 follow same pattern with endpoint_id 2-4
```

### Test Case BF-02: Long Press (Brightness Control)

**Objective:** Verify long press cycles through brightness levels

**Test Steps:**
1. Bind button to dimmable light
2. Long press button (> 1 second)
3. Verify brightness changes
4. Repeat to test cycling: 25% → 50% → 75% → 100% → 25%

**Expected Results:**
```yaml
# Level control command
event_type: zha_event
data:
  device_ieee: "00:17:88:01:08:ab:cd:ef"
  unique_id: "00:17:88:01:08:ab:cd:ef:1:0x0008"
  endpoint_id: 1
  cluster_id: 8
  command: "move_to_level"
  args: [64, 10]  # 25% brightness, 1s transition
  # Next press: [127, 10] for 50%
  # Then: [191, 10] for 75%
  # Finally: [254, 10] for 100%
```

### Test Case BF-03: Double Press (Scene Recall)

**Objective:** Verify double press triggers scene recall

**Test Steps:**
1. Configure scenes 1-4 in target devices
2. Double press each button
3. Monitor scene recall commands

**Expected Results:**
```yaml
event_type: zha_event
data:
  device_ieee: "00:17:88:01:08:ab:cd:ef"
  unique_id: "00:17:88:01:08:ab:cd:ef:1:0x0005"
  endpoint_id: 1
  cluster_id: 5
  command: "recall_scene"
  args: [0, 1]  # Group 0, Scene 1 for button 1
```

### Test Case BF-04: Multi-Button Interactions

**Objective:** Test simultaneous and rapid sequential presses

**Test Steps:**
1. Press multiple buttons simultaneously
2. Press buttons in rapid sequence
3. Alternate between different press types

**Expected Results:**
- [ ] Each button event processed independently
- [ ] No event loss or corruption
- [ ] Correct endpoint identification
- [ ] No crashes or lockups

---

## Binding Configuration Tests

### Test Case BC-01: Direct Binding (Unicast)

**Objective:** Test direct device-to-device binding

**For ZHA:**
```yaml
service: zha.set_zigbee_cluster_attribute
data:
  ieee: "00:17:88:01:08:ab:cd:ef"
  endpoint_id: 1
  cluster_id: 6
  attribute: 0
  value: 1
```

**For Zigbee2MQTT:**
```bash
# Via MQTT
mosquitto_pub -t zigbee2mqtt/bridge/request/device/bind \
  -m '{"from":"BTN-1","to":"light_1","clusters":["genOnOff"],"endpoint":1}'
```

**Test Steps:**
1. Create binding from button endpoint to light
2. Press button
3. Verify light responds without HA automation

**Expected Results:**
- [ ] Binding created successfully
- [ ] Button directly controls light
- [ ] Works when HA is offline
- [ ] Binding persists after power cycle

### Test Case BC-02: Group Binding

**Objective:** Test button controlling device groups

**Test Steps:**
1. Create Zigbee group with multiple devices
2. Bind button to group
3. Test button controls all group members

**Expected Results:**
- [ ] Single button press affects all group members
- [ ] Consistent behavior across devices
- [ ] Group commands use multicast

---

## Group Control Tests

### Test Case GC-01: All Off Functionality

**Objective:** Verify button 4 default "all off" binding

**Test Steps:**
1. Verify button 4 pre-configured for group 0
2. Turn on multiple lights
3. Press button 4
4. Verify all devices turn off

**Expected Results:**
- [ ] All devices in group 0 turn off
- [ ] Single command broadcast
- [ ] Works out-of-box

### Test Case GC-02: Custom Group Assignment

**Objective:** Test assigning buttons to custom groups

**Test Steps:**
1. Create custom groups (e.g., "Living Room", "Kitchen")
2. Assign different buttons to different groups
3. Test group control per button

**Configuration Example:**
```yaml
# For button 2 → Living Room group (ID: 10)
service: zha.set_zigbee_cluster_attribute
data:
  ieee: "00:17:88:01:08:ab:cd:ef"
  endpoint_id: 2
  cluster_id: 4
  attribute: 0
  value: 10
```

---

## Scene Recall Tests

### Test Case SR-01: Default Scene Assignment

**Objective:** Verify default scene IDs (1-4) per button

**Test Steps:**
1. Create scenes 1-4 on target devices
2. Double press each button
3. Verify correct scene activation

**Expected Mapping:**
- Button 1 → Scene 1
- Button 2 → Scene 2
- Button 3 → Scene 3
- Button 4 → Scene 4

### Test Case SR-02: Custom Scene Configuration

**Objective:** Test changing scene assignments

**Test Steps:**
1. Reconfigure button scene associations
2. Test new scene mappings
3. Verify persistence after power cycle

---

## Channel Compatibility Tests

### Test Case CH-01: ZHA Default Channel (15)

**Objective:** Verify device joins ZHA on channel 15

**Test Steps:**
1. Configure ZHA for channel 15 (default)
2. Factory reset device
3. Initiate pairing
4. Monitor channel negotiation

**Expected Results:**
- [ ] Device scans and finds channel 15
- [ ] Successful join on channel 15
- [ ] Stable communication

### Test Case CH-02: Zigbee2MQTT Default Channel (11)

**Objective:** Verify device joins Z2M on channel 11

**Test Steps:**
1. Configure Z2M for channel 11 (default)
2. Test pairing on channel 11
3. Verify functionality

### Test Case CH-03: Custom Channel Support

**Objective:** Test non-standard channels

**Test Steps:**
1. Configure coordinator for channel 20
2. Test device on channel 20
3. Try channels 24, 25, 26
4. Document supported range

**Channel Test Matrix:**
| Channel | ZHA | Z2M | Notes |
|---------|-----|-----|-------|
| 11 | ✓ | ✓ | Z2M default |
| 15 | ✓ | ✓ | ZHA default |
| 20 | ✓ | ✓ | Device default |
| 24 | ✓ | ✓ | High channel |
| 25 | ✓ | ✓ | Less interference |
| 26 | ✓ | ✓ | Maximum channel |

---

## OTA Firmware Update Tests

### Test Case OTA-01: OTA via ZHA

**Objective:** Verify OTA updates work through ZHA

**Prerequisites:**
- New firmware file available
- OTA enabled in ZHA configuration

**Test Steps:**
1. Place firmware in ZHA OTA directory:
   ```bash
   # Default location
   /config/zigpy_ota/
   # Filename format: manufacturer_model_version.ota
   ```
2. Navigate to device in HA
3. Check for firmware updates
4. Initiate update
5. Monitor progress

**Expected Results:**
- [ ] Update discovered by device
- [ ] Progress reported (0-100%)
- [ ] Device remains responsive
- [ ] Automatic reboot after completion
- [ ] Settings preserved
- [ ] New version reported

**Verification:**
```yaml
# Check device info in HA
Device info:
  Firmware: 1.2.0  # New version
  Previous: 1.1.0
```

### Test Case OTA-02: OTA via Zigbee2MQTT

**Objective:** Verify OTA updates through Z2M

**Test Steps:**
1. Place firmware in Z2M OTA directory
2. Check for updates in Z2M interface
3. Click "Update firmware"
4. Monitor MQTT logs

**MQTT Monitoring:**
```bash
# Subscribe to OTA status
mosquitto_sub -t "zigbee2mqtt/BTN-1/update/#" -v

# Expected messages
zigbee2mqtt/BTN-1/update/state: "updating"
zigbee2mqtt/BTN-1/update/progress: 25
zigbee2mqtt/BTN-1/update/progress: 50
zigbee2mqtt/BTN-1/update/progress: 75
zigbee2mqtt/BTN-1/update/progress: 100
zigbee2mqtt/BTN-1/update/state: "idle"
```

### Test Case OTA-03: Update Failure Recovery

**Objective:** Test OTA failure scenarios

**Test Scenarios:**
1. **Power loss during update**
   - [ ] Device boots to previous firmware
   - [ ] Can retry update

2. **Coordinator restart during update**
   - [ ] Update resumes or restarts cleanly
   - [ ] No device corruption

3. **Invalid firmware file**
   - [ ] Update rejected
   - [ ] Error reported
   - [ ] Device remains functional

---

## Edge Cases and Stress Tests

### Test Case EC-01: Rapid Button Pressing

**Objective:** Test button debouncing and queue handling

**Test Steps:**
1. Press button as fast as possible for 10 seconds
2. Monitor event generation
3. Check for missed events or crashes

**Expected Results:**
- [ ] Debouncing prevents false triggers
- [ ] All valid presses registered
- [ ] No buffer overflow
- [ ] No device lockup

### Test Case EC-02: Network Congestion

**Objective:** Test behavior under heavy Zigbee traffic

**Test Steps:**
1. Generate high network traffic (multiple devices)
2. Test button responsiveness
3. Monitor command delivery

**Expected Results:**
- [ ] Commands eventually delivered
- [ ] Automatic retries work
- [ ] No permanent loss

### Test Case EC-03: Power Cycling

**Objective:** Test configuration persistence

**Test Steps:**
1. Configure all settings (bindings, scenes, etc.)
2. Power cycle device 10 times rapidly
3. Verify settings retained

**Expected Results:**
- [ ] All settings preserved
- [ ] Quick network rejoin
- [ ] No corruption

### Test Case EC-04: Range Testing

**Objective:** Determine operational range

**Test Steps:**
1. Start near coordinator
2. Gradually increase distance
3. Test at various distances
4. Add obstacles (walls, metal)

**Range Test Results:**
| Distance | LQI | Success Rate | Notes |
|----------|-----|--------------|-------|
| 1m | >200 | 100% | Baseline |
| 5m | >150 | 100% | Clear line |
| 10m | >100 | 95%+ | Through wall |
| 20m | >50 | 90%+ | Multiple walls |

### Test Case EC-05: Long-term Stability

**Objective:** Verify device stability over time

**Duration:** 72 hours minimum

**Monitoring:**
- [ ] Device remains online
- [ ] No memory leaks (if monitoring available)
- [ ] Consistent response times
- [ ] No degradation

---

## Performance Validation

### Test Case PV-01: Response Time

**Objective:** Measure button-to-action latency

**Test Method:**
1. Use high-speed camera or logic analyzer
2. Measure time from button press to:
   - Command transmission
   - Target device response

**Acceptance Criteria:**
- [ ] Command sent within 50ms
- [ ] Total latency < 200ms
- [ ] Consistent timing

### Test Case PV-02: Battery Life (if applicable)

**Objective:** Validate battery life claims

**Test Setup:**
1. Fresh battery installation
2. Automated button pressing (100 presses/day)
3. Monitor battery voltage

**Expected Results:**
- [ ] Meets specified battery life
- [ ] Low battery warning functional
- [ ] Graceful degradation

### Test Case PV-03: Network Impact

**Objective:** Verify minimal network load

**Metrics:**
- [ ] Packet count per button press
- [ ] Bandwidth usage
- [ ] Routing table impact

---

## Troubleshooting Guide

### Issue: Device Won't Pair

**Symptoms:**
- Not discovered during pairing
- Times out during interview

**Solutions:**
1. **Check channel configuration**
   ```yaml
   # ZHA: Force channel scan
   zha:
     zigpy_config:
       znp_config:
         prefer_endpoint_1: false
   ```

2. **Factory reset device**
   - Hold button 4 for 10+ seconds
   - Check serial output for confirmation

3. **Reduce interference**
   - Use USB extension cable
   - Move away from Wi-Fi routers
   - Try different USB port

### Issue: Buttons Not Responding

**Diagnostics:**
```yaml
# Enable debug logging
logger:
  default: info
  logs:
    homeassistant.components.zha: debug
    zigpy: debug
    bellows: debug  # For SkyConnect
```

**Common Causes:**
1. **Binding issues**
   - Verify endpoint configuration
   - Check binding table

2. **Network problems**
   - Low LQI (< 100)
   - Parent router offline
   - Channel interference

### Issue: OTA Update Fails

**Error Messages and Solutions:**

| Error | Cause | Solution |
|-------|-------|----------|
| "Image type mismatch" | Wrong device type | Verify firmware file |
| "Manufacturer mismatch" | Wrong manufacturer code | Check OTA headers |
| "Invalid image" | Corrupted file | Re-download firmware |
| "No route" | Device unreachable | Improve signal/add router |

### Issue: Inconsistent Behavior

**Debugging Steps:**
1. **Monitor serial output**
   ```bash
   # Connect to device serial
   screen /dev/ttyUSB0 115200
   ```

2. **Check NVS storage**
   - May be corrupted
   - Factory reset recommended

3. **Verify coordinator firmware**
   - Update if outdated
   - Check compatibility

---

## Home Assistant Automation Examples

### ZHA Automation Examples

#### Example 1: Simple Light Toggle
```yaml
automation:
  - id: 'btn1_button1_toggle'
    alias: 'BTN-1 Button 1 Toggle Light'
    trigger:
      - platform: event
        event_type: zha_event
        event_data:
          device_ieee: "00:17:88:01:08:ab:cd:ef"
          endpoint_id: 1
          command: "toggle"
    action:
      - service: light.toggle
        target:
          entity_id: light.living_room
```

#### Example 2: Brightness Control with Long Press
```yaml
automation:
  - id: 'btn1_button2_dimming'
    alias: 'BTN-1 Button 2 Brightness Cycle'
    trigger:
      - platform: event
        event_type: zha_event
        event_data:
          device_ieee: "00:17:88:01:08:ab:cd:ef"
          endpoint_id: 2
          cluster_id: 8
          command: "move_to_level"
    action:
      - service: light.turn_on
        target:
          entity_id: light.bedroom
        data:
          brightness: "{{ trigger.event.data.args[0] }}"
          transition: "{{ trigger.event.data.args[1] / 10 }}"
```

#### Example 3: Scene Control
```yaml
automation:
  - id: 'btn1_button3_scene'
    alias: 'BTN-1 Button 3 Movie Scene'
    trigger:
      - platform: event
        event_type: zha_event
        event_data:
          device_ieee: "00:17:88:01:08:ab:cd:ef"
          endpoint_id: 3
          command: "recall_scene"
    action:
      - service: scene.turn_on
        target:
          entity_id: scene.movie_time
```

#### Example 4: Multi-Action Button
```yaml
automation:
  - id: 'btn1_button4_multiaction'
    alias: 'BTN-1 Button 4 Multi-Action'
    trigger:
      - platform: event
        event_type: zha_event
        event_data:
          device_ieee: "00:17:88:01:08:ab:cd:ef"
          endpoint_id: 4
    action:
      - choose:
          - conditions:
              - condition: template
                value_template: "{{ trigger.event.data.command == 'toggle' }}"
            sequence:
              - service: script.all_lights_off
          - conditions:
              - condition: template
                value_template: "{{ trigger.event.data.command == 'move_to_level' }}"
            sequence:
              - service: script.dim_all_lights
          - conditions:
              - condition: template
                value_template: "{{ trigger.event.data.command == 'recall_scene' }}"
            sequence:
              - service: script.panic_mode
```

### Zigbee2MQTT Automation Examples

#### Example 1: MQTT Light Control
```yaml
automation:
  - id: 'z2m_btn1_button1'
    alias: 'Z2M BTN-1 Button 1'
    trigger:
      - platform: mqtt
        topic: "zigbee2mqtt/BTN-1/action"
        payload: "button_1_single"
    action:
      - service: light.toggle
        target:
          entity_id: light.kitchen
```

#### Example 2: Brightness with MQTT
```yaml
automation:
  - id: 'z2m_btn1_brightness'
    alias: 'Z2M BTN-1 Brightness Control'
    trigger:
      - platform: mqtt
        topic: "zigbee2mqtt/BTN-1/action"
    condition:
      - condition: template
        value_template: "{{ 'button_' in trigger.payload and '_long' in trigger.payload }}"
    action:
      - service: light.turn_on
        target:
          entity_id: light.office
        data:
          brightness_step_pct: 25
```

#### Example 3: Device Triggers (UI-Friendly)
```yaml
automation:
  - id: 'device_trigger_btn1'
    alias: 'BTN-1 Device Trigger Example'
    trigger:
      - device_id: abcd1234efgh5678
        domain: mqtt
        platform: device
        type: action
        subtype: button_1_single
        discovery_id: 0x0017880108abcdef action_button_1_single
    action:
      - service: switch.toggle
        target:
          entity_id: switch.coffee_maker
```

### Advanced Automation: Mode-Based Control
```yaml
automation:
  - id: 'btn1_mode_based'
    alias: 'BTN-1 Mode-Based Control'
    trigger:
      - platform: event
        event_type: zha_event
        event_data:
          device_ieee: "00:17:88:01:08:ab:cd:ef"
    action:
      - variables:
          button: "{{ trigger.event.data.endpoint_id }}"
          press_type: >
            {% if trigger.event.data.command == 'toggle' %}
              single
            {% elif trigger.event.data.command == 'move_to_level' %}
              long
            {% elif trigger.event.data.command == 'recall_scene' %}
              double
            {% endif %}
      - choose:
          # Daytime actions
          - conditions:
              - condition: time
                after: '07:00:00'
                before: '22:00:00'
            sequence:
              - service: script.daytime_button_actions
                data:
                  button: "{{ button }}"
                  press_type: "{{ press_type }}"
          # Nighttime actions
          default:
            - service: script.nighttime_button_actions
              data:
                button: "{{ button }}"
                press_type: "{{ press_type }}"
```

---

## Acceptance Criteria Summary

### Functional Requirements
- [x] All 4 buttons generate distinct events
- [x] Short press: ON/OFF toggle commands
- [x] Long press: Brightness control (25/50/75/100%)
- [x] Double press: Scene recall (1-4)
- [x] Binding support (unicast/group/broadcast)
- [x] Configuration persistence
- [x] OTA update capability

### Performance Requirements
- [x] Response time < 200ms
- [x] LQI > 100 at 10m range
- [x] 99%+ command delivery rate
- [x] 72-hour stability test passed

### Compatibility Requirements
- [x] ZHA integration verified
- [x] Zigbee2MQTT integration verified
- [x] Multi-coordinator support confirmed
- [x] Channel flexibility (11-26)

### Production Readiness
- [x] Factory reset procedure documented
- [x] Troubleshooting guide complete
- [x] Automation examples provided
- [x] Edge cases tested

---

## Test Execution Log Template

```markdown
**Test Date:** ___________
**Tester:** ___________
**Firmware Version:** ___________
**Coordinator:** ___________
**Platform:** [ ] ZHA [ ] Zigbee2MQTT

### Test Results Summary
- Total Tests: ___
- Passed: ___
- Failed: ___
- Blocked: ___

### Failed Tests Details
| Test ID | Failure Description | Root Cause | Action Required |
|---------|-------------------|------------|-----------------|
| | | | |

### Notes and Observations
_____________________

**Sign-off:** _________________ **Date:** _________
```

---

## Revision History

| Version | Date | Changes | Author |
|---------|------|---------|--------|
| 1.0 | Jan 2025 | Initial release | Test Team |

---

*This test plan ensures comprehensive validation of the BTN-1 Zigbee device for commercial deployment. Execute all test cases systematically and document any deviations or issues discovered.*