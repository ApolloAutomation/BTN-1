# BTN-1 OTA (Over-The-Air) Update Implementation

## Overview

The BTN-1 Zigbee device now supports OTA firmware updates through the standard Zigbee OTA Upgrade cluster (0x0019). This implementation allows remote firmware updates without physical device access, making it easy to deploy bug fixes and new features.

## Features

### 1. Standard Zigbee OTA Protocol
- Implements OTA Upgrade server cluster (0x0019)
- Compatible with standard Zigbee coordinators and OTA tools
- Supports image notify, query, block transfer, and upgrade end commands

### 2. Safety Features
- **Version Management**: Prevents accidental downgrades (configurable)
- **Battery Check**: Ensures minimum 20% battery level before OTA (configurable)
- **Image Verification**: Validates firmware integrity before applying
- **Automatic Rollback**: ESP-IDF rollback protection on boot failure
- **NVS Preservation**: User settings and pairing data preserved during updates

### 3. Progress Tracking
- Real-time progress reporting (percentage complete)
- Status updates (downloading, verifying, applying)
- Integration with device logs for debugging

### 4. Dual Partition Support
- Utilizes ESP32-C6's dual OTA app partitions (ota_0 and ota_1)
- Each partition is 3MB, supporting large firmware images
- Seamless switching between partitions

## Version Management

Firmware versions use a 32-bit format:
- Major version (8 bits)
- Minor version (8 bits) 
- Patch version (8 bits)
- Build number (8 bits)

Current version: 1.0.0.1

## Building OTA Images

To build an OTA-capable firmware image:

```bash
# Using PlatformIO
pio run -t build

# Using ESP-IDF
idf.py build
```

The OTA binary will be located at:
- PlatformIO: `.pio/build/btn1/firmware.bin`
- ESP-IDF: `build/btn1.bin`

## Performing OTA Updates

### Using Home Assistant/Zigbee2MQTT

1. Place the new firmware binary in the OTA directory
2. The coordinator will notify the device of available updates
3. The device will download and verify the image
4. After successful verification, the device will reboot with new firmware

### Using Zigbee OTA Tools

Various Zigbee OTA tools can be used to push updates:
- Silicon Labs Simplicity Studio
- TI Z-Stack OTA tools
- Open-source Zigbee OTA servers

## OTA Attributes

The device exposes the following OTA attributes:

| Attribute | Value | Description |
|-----------|-------|-------------|
| Current File Version | 0x01000001 | Current firmware version |
| Manufacturer ID | 0x1234 | Apollo Automation (example) |
| Image Type | 0x0001 | BTN-1 device type |
| Image Status | 0x00 | Normal operation |

## Status Codes

The OTA handler reports the following status codes:

- `IDLE`: No OTA in progress
- `DOWNLOADING`: Receiving firmware image
- `VERIFYING`: Validating image integrity
- `APPLYING`: Setting boot partition
- `SUCCESS`: Update completed successfully
- `ERROR_INVALID_IMAGE`: Corrupted or invalid firmware
- `ERROR_VERSION`: Version check failed
- `ERROR_BATTERY_LOW`: Insufficient battery level
- `ERROR_WRITE_FAILED`: Flash write error
- `ERROR_VERIFY_FAILED`: Image verification failed

## Configuration

OTA behavior can be configured in `main.c`:

```c
ota_config_t ota_config = {
    .min_battery_percent = 20,      // Minimum battery level
    .allow_downgrade = false,        // Prevent downgrades
    .chunk_size = 1024,             // Transfer chunk size
    .timeout_ms = 300000            // 5 minute timeout
};
```

## Logs

During OTA updates, the device provides detailed logging:

```
I (12345) OTA_HANDLER: Current firmware version: 1.0.0.1
I (12346) OTA_HANDLER: Build timestamp: Oct 14 2024 12:34:56
I (20000) OTA_HANDLER: Starting OTA update to partition ota_1, size: 2097152 bytes
I (20100) OTA_HANDLER: OTA Progress: 102400/2097152 bytes (5%)
I (25000) OTA_HANDLER: OTA Progress: 1048576/2097152 bytes (50%)
I (30000) OTA_HANDLER: OTA Progress: 2097152/2097152 bytes (100%)
I (30100) OTA_HANDLER: New firmware version: 1.0.1.0
I (30200) OTA_HANDLER: OTA update successful. Restart to boot new firmware.
```

## Security Considerations

1. **Image Signing**: Consider implementing firmware signing for production
2. **Encrypted Updates**: OTA data can be encrypted if needed
3. **Secure Boot**: Enable ESP32-C6 secure boot for additional protection

## Troubleshooting

### OTA Fails to Start
- Check battery level (must be >20% or USB connected)
- Verify Zigbee network connection
- Ensure coordinator supports OTA cluster

### OTA Fails During Transfer
- Check for network stability
- Verify image size fits in partition (3MB max)
- Monitor device logs for specific errors

### Device Doesn't Boot After OTA
- Automatic rollback will restore previous firmware
- Check serial logs for boot errors
- Verify image was built for correct target

## Future Enhancements

Potential improvements for production:
1. Differential updates to reduce transfer size
2. Compression support for smaller images
3. Resume capability for interrupted transfers
4. A/B testing support for gradual rollouts