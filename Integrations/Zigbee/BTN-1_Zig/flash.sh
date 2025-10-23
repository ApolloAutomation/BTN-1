#!/bin/bash
# BTN-1 Zigbee Firmware Flash Script for Linux/macOS

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
DEFAULT_PORT="/dev/ttyUSB0"
PORT=${1:-$DEFAULT_PORT}

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}   BTN-1 Zigbee Firmware Flash Tool${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Check if ESP-IDF is sourced
if [ -z "$IDF_PATH" ]; then
    echo -e "${YELLOW}ESP-IDF not sourced. Attempting to source...${NC}"
    if [ -f "$HOME/esp/esp-idf/export.sh" ]; then
        source "$HOME/esp/esp-idf/export.sh"
    else
        echo -e "${RED}Error: ESP-IDF not found at $HOME/esp/esp-idf${NC}"
        echo "Please install ESP-IDF first:"
        echo "  git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git ~/esp/esp-idf"
        echo "  cd ~/esp/esp-idf && ./install.sh esp32c6"
        exit 1
    fi
fi

# Detect port if not specified
if [ "$PORT" = "$DEFAULT_PORT" ]; then
    echo "Detecting USB port..."
    if [ "$(uname)" = "Darwin" ]; then
        # macOS
        DETECTED_PORT=$(ls /dev/cu.usbserial-* 2>/dev/null | head -n1)
        if [ -z "$DETECTED_PORT" ]; then
            DETECTED_PORT=$(ls /dev/cu.SLAB_USBtoUART 2>/dev/null | head -n1)
        fi
    else
        # Linux
        DETECTED_PORT=$(ls /dev/ttyUSB* 2>/dev/null | head -n1)
        if [ -z "$DETECTED_PORT" ]; then
            DETECTED_PORT=$(ls /dev/ttyACM* 2>/dev/null | head -n1)
        fi
    fi
    
    if [ ! -z "$DETECTED_PORT" ]; then
        PORT=$DETECTED_PORT
        echo -e "${GREEN}Detected port: $PORT${NC}"
    fi
fi

# Check if port exists
if [ ! -e "$PORT" ]; then
    echo -e "${RED}Error: Port $PORT not found${NC}"
    echo "Available ports:"
    ls /dev/tty* 2>/dev/null | grep -E "(USB|ACM|usbserial)" || echo "  No USB devices found"
    echo ""
    echo "Usage: $0 [port]"
    echo "Example: $0 /dev/ttyUSB0"
    exit 1
fi

# Check if we have permission
if [ ! -w "$PORT" ]; then
    echo -e "${YELLOW}Warning: No write permission for $PORT${NC}"
    echo "Trying to fix permissions..."
    sudo chmod 666 "$PORT" 2>/dev/null
    if [ ! -w "$PORT" ]; then
        echo -e "${RED}Error: Cannot access $PORT${NC}"
        echo "Try: sudo usermod -a -G dialout $USER"
        echo "Then logout and login again"
        exit 1
    fi
fi

echo -e "${YELLOW}Using port: $PORT${NC}"
echo ""

# Build and flash
echo -e "${GREEN}Step 1: Setting target to ESP32-C6${NC}"
idf.py set-target esp32c6

echo ""
echo -e "${GREEN}Step 2: Building firmware${NC}"
idf.py build

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}Step 3: Flashing firmware${NC}"
echo "If this fails, try:"
echo "  1. Hold BOOT button"
echo "  2. Press RESET button"  
echo "  3. Release BOOT button"
echo ""

idf.py -p $PORT flash

if [ $? -ne 0 ]; then
    echo -e "${RED}Flash failed!${NC}"
    echo "Troubleshooting:"
    echo "  - Check USB cable is data-capable (not charge-only)"
    echo "  - Try manual boot mode (see above)"
    echo "  - Try slower baud: idf.py -p $PORT -b 115200 flash"
    exit 1
fi

echo ""
echo -e "${GREEN}Step 4: Opening serial monitor${NC}"
echo "Press Ctrl+] to exit monitor"
echo ""

idf.py -p $PORT monitor

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Flash complete! BTN-1 is ready.${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Next steps:"
echo "  1. Hold Button 1 for 10 seconds to enter pairing mode"
echo "  2. Enable 'Permit Join' in Home Assistant ZHA"
echo "  3. Device will automatically join the network"