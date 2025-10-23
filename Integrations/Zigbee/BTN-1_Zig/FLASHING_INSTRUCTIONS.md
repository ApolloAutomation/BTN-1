# BTN-1 Zigbee Firmware Flashing Instructions

## Prerequisites

### Required Hardware
- BTN-1 device (ESP32-C6 based)
- USB-C cable (data capable, not charge-only)
- Computer with USB port

### Required Software

#### 1. ESP-IDF Installation
```bash
# Install ESP-IDF v5.1.2 or later (required for ESP32-C6 support)
mkdir -p ~/esp
cd ~/esp
git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32c6
. ./export.sh
```

#### 2. Additional Tools
```bash
# Install esptool (if not included with ESP-IDF)
pip install esptool

# Install ESP Zigbee SDK
cd ~/esp
git clone https://github.com/espressif/esp-zigbee-sdk.git
```

## Building the Firmware

### Step 1: Navigate to Project Directory
```bash
cd BTN-1/Integrations/Zigbee/BTN-1_Zig
```

### Step 2: Configure the Project
```bash
# Set target to ESP32-C6
idf.py set-target esp32c6

# Optional: Configure project settings
idf.py menuconfig
```

### Step 3: Build the Firmware
```bash
# Clean build (recommended for first time)
idf.py fullclean
idf.py build
```

The build process will create:
- `build/BTN-1_Zig.bin` - Main application binary
- `build/bootloader/bootloader.bin` - Bootloader
- `build/partition_table/partition-table.bin` - Partition table

## Flashing Methods

### Method 1: Using ESP-IDF (Recommended)

```bash
# Auto-detect port and flash
idf.py -p /dev/ttyUSB0 flash monitor

# Or specify port explicitly
# Linux/Mac:
idf.py -p /dev/ttyUSB0 flash
# Windows:
idf.py -p COM3 flash

# Flash and open serial monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

### Method 2: Using esptool.py Directly

```bash
# Full flash including bootloader and partition table
esptool.py --chip esp32c6 \
    --port /dev/ttyUSB0 \
    --baud 460800 \
    --before default_reset \
    --after hard_reset \
    write_flash -z \
    --flash_mode dio \
    --flash_freq 80m \
    --flash_size 4MB \
    0x0 build/bootloader/bootloader.bin \
    0x8000 build/partition_table/partition-table.bin \
    0x10000 build/BTN-1_Zig.bin
```

### Method 3: Using Web Flasher (Easiest for End Users)

1. Open Chrome/Edge browser
2. Navigate to https://espressif.github.io/esptool-js/
3. Click "Connect" and select your BTN-1 device
4. Load the firmware files:
   - Offset 0x0: bootloader.bin
   - Offset 0x8000: partition-table.bin
   - Offset 0x10000: BTN-1_Zig.bin
5. Click "Program"

## Platform-Specific Instructions

### Linux

1. **Add user to dialout group** (for USB access):
```bash
sudo usermod -a -G dialout $USER
# Log out and back in for changes to take effect
```

2. **Find the device port**:
```bash
ls /dev/ttyUSB* /dev/ttyACM*
# Usually /dev/ttyUSB0 or /dev/ttyACM0
```

### macOS

1. **Install USB drivers** (if needed):
```bash
# Install Silicon Labs CP210x driver for some USB chips
brew install --cask silicon-labs-vcp-driver
```

2. **Find the device port**:
```bash
ls /dev/cu.*
# Look for /dev/cu.usbserial-* or /dev/cu.SLAB_USBtoUART
```

### Windows

1. **Install USB drivers**:
   - Download from Silicon Labs or FTDI depending on your USB chip
   - Windows 10/11 usually auto-installs drivers

2. **Find the COM port**:
   - Open Device Manager
   - Look under "Ports (COM & LPT)"
   - Note the COM port number (e.g., COM3)

## Flashing Process

### Pre-Flash Checklist

- [ ] BTN-1 is connected via USB
- [ ] Correct port is identified
- [ ] No other applications using the serial port
- [ ] Firmware files are built successfully

### Entering Download Mode

The BTN-1 should automatically enter download mode when flashing. If it doesn't:

1. **Manual Boot Mode** (if automatic fails):
   - Hold the BOOT button
   - Press and release the RESET button
   - Release the BOOT button
   - The device is now in download mode

2. **Alternative**: Some boards auto-reset via USB

### Flash the Firmware

```bash
# Navigate to project directory
cd BTN-1/Integrations/Zigbee/BTN-1_Zig

# Flash with monitoring
idf.py -p /dev/ttyUSB0 flash monitor
```

### Expected Output
```
Connecting........_
Chip is ESP32-C6 (revision v0.0)
Features: WiFi 6, BLE, 802.15.4
Crystal is 40MHz
MAC: xx:xx:xx:xx:xx:xx
Uploading stub...
Running stub...
Stub running...
Changing baud rate to 460800
Changed.
Configuring flash size...
Flash will be erased from 0x00000000 to 0x00005fff...
Flash will be erased from 0x00008000 to 0x00008fff...
Flash will be erased from 0x00010000 to 0x000a0fff...
Compressed 23536 bytes to 14768...
Wrote 23536 bytes (14768 compressed) at 0x00000000 in 0.7 seconds...
Hash of data verified.
Compressed 3072 bytes to 103...
Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds...
Hash of data verified.
Compressed 589824 bytes to 367890...
Wrote 589824 bytes (367890 compressed) at 0x00010000 in 8.9 seconds...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```

## Post-Flash Verification

### Serial Monitor Output

After successful flashing, you should see:
```
=== BTN-1 Zigbee Firmware ===
Version: 1.0.0.1
Build: <timestamp>
=============================
LED indicator system initialized
Button handler initialized and started
=== Button Controls ===
Button 1 Long Press (10s): Enter pairing mode
All 4 Buttons (5s): Factory reset
Short Press: Toggle lights on/off
Long Press (0.5s): Adjust brightness
Double Press: Recall scene
========================
```

### LED Indicators

On successful boot:
- **Double flash**: Device ready (startup pattern)
- **No LED**: Device idle, waiting for commands

### Testing Basic Functions

1. **Test button press**:
   - Press any button briefly
   - LED should flash quickly (50ms)

2. **Check pairing mode**:
   - Hold Button 1 for 10 seconds
   - LED should start fast blinking (searching)

3. **Verify factory reset**:
   - Hold all 4 buttons for 5 seconds
   - LED should flash rapidly (20 times)
   - Device will restart

## Troubleshooting

### Common Issues

#### 1. "Port not found" Error
```bash
# Linux: Check permissions
sudo chmod 666 /dev/ttyUSB0
# Or add user to dialout group

# All platforms: Verify port
# Linux/Mac:
ls /dev/tty*
# Windows:
mode
```

#### 2. "Failed to connect" Error
- Try lower baud rate: `--baud 115200`
- Manually enter boot mode (hold BOOT, press RESET)
- Check USB cable (must be data cable, not charge-only)

#### 3. "Chip sync error"
- Ensure correct chip type: `--chip esp32c6`
- Try different USB port
- Reset device and try again

#### 4. Build Errors
```bash
# Clean and rebuild
idf.py fullclean
idf.py reconfigure
idf.py build
```

### Advanced Flashing Options

#### Erase Flash Completely
```bash
# Erase entire flash (factory reset)
esptool.py --chip esp32c6 --port /dev/ttyUSB0 erase_flash

# Then flash firmware
idf.py -p /dev/ttyUSB0 flash
```

#### Flash Individual Partitions
```bash
# Flash only application
esptool.py --chip esp32c6 --port /dev/ttyUSB0 write_flash 0x10000 build/BTN-1_Zig.bin

# Flash only bootloader
esptool.py --chip esp32c6 --port /dev/ttyUSB0 write_flash 0x0 build/bootloader/bootloader.bin
```

#### Backup Current Firmware
```bash
# Read flash to file
esptool.py --chip esp32c6 --port /dev/ttyUSB0 read_flash 0x0 0x400000 backup.bin
```

## Quick Flash Script

Create a flash script for repeated use:

### Linux/Mac: `flash.sh`
```bash
#!/bin/bash
PORT=${1:-/dev/ttyUSB0}
echo "Flashing BTN-1 on port $PORT..."
cd BTN-1/Integrations/Zigbee/BTN-1_Zig
idf.py -p $PORT flash monitor
```

### Windows: `flash.bat`
```batch
@echo off
set PORT=%1
if "%PORT%"=="" set PORT=COM3
echo Flashing BTN-1 on port %PORT%...
cd BTN-1\Integrations\Zigbee\BTN-1_Zig
idf.py -p %PORT% flash monitor
```

## After Flashing

Once the device is flashed:

1. **LED shows startup pattern** (double flash)
2. **Device is ready for pairing** with Home Assistant
3. **Follow the pairing instructions** in PAIRING_INSTRUCTIONS.md
4. **Test button functions** to ensure proper operation

## Support

If you encounter issues:
1. Check serial monitor output for error messages
2. Verify ESP-IDF version compatibility
3. Ensure Zigbee SDK is properly installed
4. Try erasing flash completely before reflashing
5. Check the GitHub repository for known issues

---
*For Home Assistant pairing instructions, see PAIRING_INSTRUCTIONS.md*