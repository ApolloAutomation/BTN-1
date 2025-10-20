# BTN-1 Zigbee Firmware Development Todo List

## 1. Development Environment Setup
- [ ] Install ESP-IDF v5.1+ with ESP32-C6 support
- [ ] Set up ESP-IDF environment variables and PATH
- [ ] Install ESP-IDF VS Code extension
- [ ] Configure IDF_TARGET=esp32c6 in environment
- [ ] Install Zigbee sniffer tools (Wireshark with zboss plugin)
- [ ] Set up JTAG debugging for ESP32-C6
- [ ] Install Python dependencies for ESP-IDF Zigbee tools

## 2. Project Configuration
- [ ] Create new ESP-IDF project structure (remove PlatformIO setup)
- [ ] Configure sdkconfig for ESP32-C6 with Zigbee stack enabled
- [ ] Set up partition table for Zigbee requirements (NVS, Zigbee data)
- [ ] Configure menuconfig for IEEE 802.15.4 and Zigbee support
- [ ] Enable CONFIG_ZB_ENABLED and CONFIG_ZB_ZCZR (Coordinator/Router)
- [ ] Set up CMakeLists.txt with proper Zigbee components
- [ ] Configure flash size and memory allocation for Zigbee stack

## 3. Core Zigbee Implementation
- [ ] Initialize Zigbee stack with esp_zb_init()
- [ ] Configure device as Zigbee End Device (ZED)
- [ ] Set up manufacturer info and device identifiers
- [ ] Implement Zigbee device descriptor (Model ID: BTN-1)
- [ ] Create endpoint for button controller device type
- [ ] Implement basic cluster support (Basic, Identify, Groups, Scenes)
- [ ] Configure network parameters (PAN ID, channel mask)
- [ ] Implement device commissioning and network joining

## 4. Button Hardware Integration
- [ ] Configure GPIO pins for 4 buttons with internal pull-ups
- [ ] Implement interrupt-based button detection
- [ ] Add debouncing logic for button presses
- [ ] Detect short press vs long press events
- [ ] Implement multi-press detection (double/triple click)
- [ ] Create button event queue for Zigbee command processing
- [ ] Add LED feedback for button press confirmation

## 5. Zigbee Command Implementation
- [ ] Implement On/Off cluster client commands
- [ ] Add Level Control cluster for dimming functionality
- [ ] Implement Scene cluster commands (store/recall scenes)
- [ ] Add Group cluster support for multi-device control
- [ ] Create command builder for different button actions
- [ ] Implement attribute reporting for button states
- [ ] Add support for binding table configuration
- [ ] Implement broadcast/multicast command support

## 6. Home Assistant Integration
- [ ] Configure device to be ZHA compatible
- [ ] Implement standard Home Assistant device triggers
- [ ] Add device configuration cluster for HA discovery
- [ ] Support configuration via HA UI (button mappings)
- [ ] Implement OTA cluster for firmware updates via HA
- [ ] Add diagnostic cluster for HA device info
- [ ] Test with ZHA quirks if needed
- [ ] Verify device appears correctly in HA UI

## 7. Testing & Debugging
- [ ] Set up Zigbee network with coordinator for testing
- [ ] Implement debug logging for Zigbee events
- [ ] Test network joining/leaving procedures
- [ ] Verify all button commands work correctly
- [ ] Test group and scene functionality
- [ ] Measure and log RSSI/LQI values
- [ ] Test with multiple Zigbee devices
- [ ] Stress test with rapid button presses

## 8. Power Management Optimization
- [ ] Implement Zigbee End Device sleep modes
- [ ] Configure wake-up on button press
- [ ] Optimize radio duty cycle for battery life
- [ ] Implement battery level reporting cluster
- [ ] Add low battery warnings
- [ ] Test and measure actual power consumption
- [ ] Optimize Zigbee polling intervals
- [ ] Implement adaptive polling based on activity

## 9. Documentation & Deployment
- [ ] Create user manual for button configuration
- [ ] Document Home Assistant setup process
- [ ] Write firmware flashing instructions
- [ ] Create troubleshooting guide
- [ ] Document button mapping options
- [ ] Prepare release notes and changelog
- [ ] Set up GitHub Actions for automated builds
- [ ] Create OTA update packages for distribution