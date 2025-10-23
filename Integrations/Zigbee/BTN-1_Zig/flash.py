#!/usr/bin/env python3
"""
BTN-1 Zigbee Firmware Flash Tool
Cross-platform Python utility for flashing BTN-1 devices
"""

import sys
import os
import subprocess
import platform
import serial.tools.list_ports
import argparse
import time
from pathlib import Path

# ANSI color codes for terminal output
class Colors:
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    NC = '\033[0m'  # No Color

def print_colored(text, color=Colors.NC):
    """Print colored text to terminal"""
    print(f"{color}{text}{Colors.NC}")

def find_esp32_ports():
    """Find likely ESP32 ports"""
    esp_ports = []
    ports = serial.tools.list_ports.comports()
    
    for port in ports:
        # Check for common ESP32 USB-UART chips
        if any(chip in str(port.description).lower() for chip in ['cp210', 'ch340', 'ftdi', 'esp32']):
            esp_ports.append(port.device)
        # Check vendor IDs
        if port.vid in [0x10C4, 0x1A86, 0x0403]:  # CP210x, CH340, FTDI
            if port.device not in esp_ports:
                esp_ports.append(port.device)
    
    return esp_ports

def check_esp_idf():
    """Check if ESP-IDF is available"""
    try:
        result = subprocess.run(['idf.py', '--version'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            return True
    except (FileNotFoundError, subprocess.SubprocessError):
        pass
    
    return False

def find_esp_idf():
    """Try to find ESP-IDF installation"""
    system = platform.system()
    possible_paths = []
    
    if system == "Windows":
        possible_paths = [
            Path.home() / "esp" / "esp-idf",
            Path("C:/esp/esp-idf"),
            Path("C:/Espressif/frameworks/esp-idf")
        ]
    else:  # Linux/macOS
        possible_paths = [
            Path.home() / "esp" / "esp-idf",
            Path("/opt/esp-idf"),
            Path("/usr/local/esp-idf")
        ]
    
    for path in possible_paths:
        if path.exists() and (path / "export.sh").exists():
            return path
    
    return None

def setup_esp_idf_env():
    """Setup ESP-IDF environment"""
    esp_idf_path = find_esp_idf()
    if not esp_idf_path:
        return False
    
    system = platform.system()
    if system == "Windows":
        export_script = esp_idf_path / "export.bat"
        # For Windows, we need to run in the same shell
        os.environ['IDF_PATH'] = str(esp_idf_path)
        # Add tools to PATH
        tools_path = esp_idf_path / "tools"
        if tools_path.exists():
            os.environ['PATH'] = f"{tools_path};{os.environ['PATH']}"
    else:
        export_script = esp_idf_path / "export.sh"
        # Source the export script
        command = f"source {export_script} && env"
        proc = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
        for line in proc.stdout:
            (key, _, value) = line.decode().partition("=")
            os.environ[key] = value.rstrip()
    
    return check_esp_idf()

def run_command(cmd, description=""):
    """Run a command and handle output"""
    if description:
        print_colored(f"\n{description}", Colors.GREEN)
    
    print(f"Running: {' '.join(cmd)}")
    
    try:
        process = subprocess.Popen(cmd, stdout=subprocess.PIPE, 
                                 stderr=subprocess.STDOUT, 
                                 universal_newlines=True)
        
        for line in process.stdout:
            print(line, end='')
        
        process.wait()
        return process.returncode == 0
        
    except Exception as e:
        print_colored(f"Error: {e}", Colors.RED)
        return False

def flash_with_idf(port, monitor=True):
    """Flash using ESP-IDF"""
    # Set target
    if not run_command(['idf.py', 'set-target', 'esp32c6'], 
                      "Step 1: Setting target to ESP32-C6"):
        return False
    
    # Build
    if not run_command(['idf.py', 'build'], 
                      "Step 2: Building firmware"):
        return False
    
    # Flash
    print_colored("\nStep 3: Flashing firmware", Colors.GREEN)
    print("If this fails, try:")
    print("  1. Hold BOOT button")
    print("  2. Press RESET button")
    print("  3. Release BOOT button\n")
    
    if not run_command(['idf.py', '-p', port, 'flash']):
        return False
    
    # Monitor
    if monitor:
        print_colored("\nStep 4: Opening serial monitor", Colors.GREEN)
        print("Press Ctrl+] to exit monitor\n")
        run_command(['idf.py', '-p', port, 'monitor'])
    
    return True

def flash_with_esptool(port, firmware_path):
    """Flash using esptool directly"""
    print_colored("Using esptool for direct flash", Colors.YELLOW)
    
    try:
        import esptool
    except ImportError:
        print_colored("esptool not found. Installing...", Colors.YELLOW)
        subprocess.run([sys.executable, '-m', 'pip', 'install', 'esptool'])
        import esptool
    
    # Find firmware binaries
    build_dir = Path("build")
    if not build_dir.exists():
        print_colored("Build directory not found. Please build first.", Colors.RED)
        return False
    
    # Flash command
    cmd = [
        sys.executable, '-m', 'esptool',
        '--chip', 'esp32c6',
        '--port', port,
        '--baud', '460800',
        '--before', 'default_reset',
        '--after', 'hard_reset',
        'write_flash', '-z',
        '--flash_mode', 'dio',
        '--flash_freq', '80m',
        '--flash_size', 'detect'
    ]
    
    # Add binary files (typical ESP-IDF layout)
    binaries = [
        ('0x0', 'bootloader/bootloader.bin'),
        ('0x8000', 'partition_table/partition-table.bin'),
        ('0x10000', 'BTN-1_Zig.bin')
    ]
    
    for addr, file in binaries:
        file_path = build_dir / file
        if file_path.exists():
            cmd.extend([addr, str(file_path)])
        else:
            print_colored(f"Warning: {file} not found", Colors.YELLOW)
    
    return run_command(cmd, "Flashing with esptool")

def main():
    parser = argparse.ArgumentParser(description='BTN-1 Zigbee Firmware Flash Tool')
    parser.add_argument('port', nargs='?', help='Serial port (e.g., COM3, /dev/ttyUSB0)')
    parser.add_argument('--no-monitor', action='store_true', 
                       help='Skip serial monitor after flashing')
    parser.add_argument('--esptool', action='store_true',
                       help='Use esptool directly instead of ESP-IDF')
    parser.add_argument('--firmware', help='Path to pre-built firmware directory')
    
    args = parser.parse_args()
    
    print_colored("========================================", Colors.GREEN)
    print_colored("   BTN-1 Zigbee Firmware Flash Tool", Colors.GREEN)
    print_colored("========================================", Colors.GREEN)
    print()
    
    # Find or validate port
    if args.port:
        port = args.port
        print_colored(f"Using specified port: {port}", Colors.YELLOW)
    else:
        print("Detecting ESP32 devices...")
        esp_ports = find_esp32_ports()
        
        if not esp_ports:
            print_colored("No ESP32 devices found!", Colors.RED)
            print("\nPlease specify port manually:")
            print("  python flash.py COM3        (Windows)")
            print("  python flash.py /dev/ttyUSB0 (Linux)")
            print("  python flash.py /dev/cu.usbserial-0001 (macOS)")
            sys.exit(1)
        
        if len(esp_ports) == 1:
            port = esp_ports[0]
            print_colored(f"Found ESP32 on port: {port}", Colors.GREEN)
        else:
            print("Multiple ESP32 devices found:")
            for i, p in enumerate(esp_ports, 1):
                print(f"  {i}. {p}")
            
            while True:
                try:
                    choice = input("\nSelect port (1-{}): ".format(len(esp_ports)))
                    idx = int(choice) - 1
                    if 0 <= idx < len(esp_ports):
                        port = esp_ports[idx]
                        break
                except (ValueError, IndexError):
                    pass
                print_colored("Invalid choice. Please try again.", Colors.RED)
    
    # Check port exists
    if not Path(port).exists() and platform.system() != "Windows":
        print_colored(f"Warning: Port {port} does not exist", Colors.YELLOW)
        response = input("Continue anyway? (y/N): ")
        if response.lower() != 'y':
            sys.exit(1)
    
    # Flash based on method
    success = False
    
    if args.esptool or args.firmware:
        # Direct esptool flash
        firmware_path = args.firmware if args.firmware else "."
        success = flash_with_esptool(port, firmware_path)
    else:
        # Try ESP-IDF first
        if not check_esp_idf():
            print_colored("ESP-IDF not found in PATH", Colors.YELLOW)
            print("Attempting to locate ESP-IDF installation...")
            
            if not setup_esp_idf_env():
                print_colored("\nESP-IDF not available!", Colors.RED)
                print("\nOptions:")
                print("1. Install ESP-IDF:")
                print("   https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/get-started/")
                print("\n2. Use pre-built firmware:")
                print("   python flash.py --esptool --firmware /path/to/firmware")
                print("\n3. Use web flasher:")
                print("   https://apollo-btn1-flasher.github.io")
                sys.exit(1)
        
        success = flash_with_idf(port, monitor=not args.no_monitor)
    
    if success:
        print_colored("\n========================================", Colors.GREEN)
        print_colored("Flash complete! BTN-1 is ready.", Colors.GREEN)
        print_colored("========================================", Colors.GREEN)
        print("\nNext steps:")
        print("  1. Hold Button 1 for 10 seconds to enter pairing mode")
        print("  2. Enable 'Permit Join' in Home Assistant ZHA")
        print("  3. Device will automatically join the network")
    else:
        print_colored("\n========================================", Colors.RED)
        print_colored("Flash failed!", Colors.RED)
        print_colored("========================================", Colors.RED)
        print("\nTroubleshooting:")
        print("  - Check USB cable is data-capable (not charge-only)")
        print("  - Try manual boot mode (BOOT + RESET buttons)")
        print("  - Verify correct port selection")
        print("  - Install USB drivers if needed")
        sys.exit(1)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print_colored("\n\nFlash cancelled by user", Colors.YELLOW)
        sys.exit(0)
    except Exception as e:
        print_colored(f"\nUnexpected error: {e}", Colors.RED)
        sys.exit(1)