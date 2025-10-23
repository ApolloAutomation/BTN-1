# BTN-1 Zigbee Firmware Flash Troubleshooting

## Device Stuck in Download Mode

If you see "waiting for download" in the serial output, the device is stuck in download mode.

### Solution Steps:

1. **Physical Reset Method**:
   ```bash
   # Unplug the USB cable
   # Hold down the BOOT button (GPIO 9)
   # Plug in the USB cable while holding BOOT
   # Release BOOT after 1 second
   # Now try flashing:
   idf.py -p /dev/cu.usbmodem1101 flash monitor
   ```

2. **Alternative Flash Method**:
   ```bash
   # Use esptool directly with manual reset:
   esptool.py --chip esp32c6 -p /dev/cu.usbmodem1101 -b 115200 \
     --before default_reset --after hard_reset write_flash \
     --flash_mode dio --flash_freq 80m --flash_size 8MB \
     0x0 build/bootloader/bootloader.bin \
     0x8000 build/partition_table/partition-table.bin \
     0x10000 build/BTN-1_Zig.bin
   ```

3. **Lower Baud Rate Method**:
   ```bash
   # Sometimes lower baud rate helps with connection issues:
   idf.py -p /dev/cu.usbmodem1101 -b 115200 flash monitor
   ```

## Common Issues and Solutions

### Serial Port Busy
- Close any other serial monitors (Arduino IDE, etc.)
- Check for other processes using the port:
  ```bash
  lsof | grep usbmodem
  ```

### Device Not Responding
1. Check cable - use a data cable, not charge-only
2. Try different USB port
3. Check device power - ensure 3.3V supply is stable

### After Successful Flash

The device should show:
```
I (xxx) BTN1_ZIGBEE: === BTN-1 Zigbee Firmware ===
I (xxx) SIMPLE_LED: Initializing simple LED on GPIO 8
I (xxx) BUTTON_HANDLER: Button handler started
```

## LED Indication Patterns

Once running, you'll see these LED patterns:
- **Startup**: Brief flash
- **Button Press**: Quick flash acknowledgment
- **Pairing Mode** (10s hold): Fast blink
- **Joining Network**: Slow blink
- **Connected**: Solid 3 seconds
- **Error**: Double blink pattern
- **Factory Reset**: Rapid flash