# BTN-1 Quick Flash Guide

## 🚀 Fastest Method (Pre-compiled)

If you have pre-compiled binaries:

```bash
# Connect BTN-1 via USB, then run:
esptool.py --chip esp32c6 --port /dev/ttyUSB0 --baud 460800 \
  write_flash -z --flash_mode dio --flash_size 4MB \
  0x0 bootloader.bin \
  0x8000 partition-table.bin \
  0x10000 BTN-1_Zig.bin
```

**Windows**: Replace `/dev/ttyUSB0` with `COM3` (or your port)  
**macOS**: Replace with `/dev/cu.usbserial-*`

## 📦 Building from Source

### 1. Setup ESP-IDF
```bash
# One-time setup
git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git ~/esp/esp-idf
cd ~/esp/esp-idf
./install.sh esp32c6
```

### 2. Build & Flash
```bash
# Every time you flash
source ~/esp/esp-idf/export.sh
cd BTN-1/Integrations/Zigbee/BTN-1_Zig
idf.py set-target esp32c6
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## 🌐 Web Flasher (No Install Required)

1. Open: https://espressif.github.io/esptool-js/
2. Connect BTN-1 via USB
3. Click "Connect" → Select device
4. Add files:
   - `0x0` → bootloader.bin
   - `0x8000` → partition-table.bin  
   - `0x10000` → BTN-1_Zig.bin
5. Click "Program"

## ✅ Success Indicators

After flashing, you should see:
- **LED**: Double flash (startup)
- **Serial**: "BTN-1 Zigbee Firmware" message
- **Buttons**: Quick LED flash when pressed

## 🔧 Troubleshooting

**Can't connect?**
- Hold BOOT button, press RESET, release BOOT
- Try slower baud: `--baud 115200`
- Check it's a data USB cable

**Wrong port?**
- Linux: `ls /dev/ttyUSB*`
- macOS: `ls /dev/cu.*`
- Windows: Check Device Manager → COM Ports

**Permission denied?**
- Linux: `sudo chmod 666 /dev/ttyUSB0`
- Or: `sudo usermod -a -G dialout $USER` (then logout/login)

---
*Next: See QUICK_PAIRING_GUIDE.md for Home Assistant setup*