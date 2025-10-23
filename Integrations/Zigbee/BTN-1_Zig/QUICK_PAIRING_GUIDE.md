# BTN-1 Quick Pairing Guide for Home Assistant ZHA

## 🚀 Quick Start

### First-Time Pairing

1. **Home Assistant**: Settings → ZHA → Add Device (starts 60-second search)
2. **BTN-1**: Hold Button 1 for 10 seconds (watch for LED feedback)
3. **Wait**: Device will appear in HA within 30 seconds
4. **Configure**: Name device and assign to room

### LED Status Guide

- ⚡ **Fast Blink** = Searching for network
- 🔄 **Slow Blink** = Joining network  
- ✅ **Solid 3s** = Successfully paired
- ❌ **Double Blink** = Error (try again)

### Button Controls

| Press Type | Action |
|------------|--------|
| **Tap** | ON/OFF Toggle |
| **Hold 0.5s** | Brightness (25%→50%→75%→100%) |
| **Double Tap** | Scene Recall |

### Special Functions

- **🔄 Pairing Mode**: Hold Button 1 for 10 seconds
- **🏭 Factory Reset**: Hold ALL 4 buttons for 5 seconds

## Troubleshooting

**Not pairing?**
- Move closer to coordinator
- Ensure ZHA "Permit Join" is active
- Try factory reset first

**Paired but not working?**
- Check Developer Tools → States for button entities
- Power cycle the device
- Re-bind in ZHA device configuration

---
*Full documentation: PAIRING_INSTRUCTIONS.md*