@echo off
REM BTN-1 Zigbee Firmware Flash Script for Windows

setlocal enabledelayedexpansion

REM Colors (Windows 10+)
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set NC=[0m

echo %GREEN%========================================%NC%
echo %GREEN%   BTN-1 Zigbee Firmware Flash Tool%NC%
echo %GREEN%========================================%NC%
echo.

REM Default port
set DEFAULT_PORT=COM3
if "%1"=="" (
    set PORT=%DEFAULT_PORT%
) else (
    set PORT=%1
)

REM Check if ESP-IDF is available
where idf.py >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo %YELLOW%ESP-IDF not found in PATH%NC%
    echo Looking for ESP-IDF installation...
    
    if exist "%USERPROFILE%\esp\esp-idf\export.bat" (
        echo Found ESP-IDF, loading environment...
        call "%USERPROFILE%\esp\esp-idf\export.bat"
    ) else if exist "C:\esp\esp-idf\export.bat" (
        echo Found ESP-IDF, loading environment...
        call "C:\esp\esp-idf\export.bat"
    ) else (
        echo %RED%Error: ESP-IDF not found%NC%
        echo Please install ESP-IDF first:
        echo   1. Download installer from https://dl.espressif.com/dl/esp-idf/
        echo   2. Install with ESP32-C6 support
        echo   3. Run this script from ESP-IDF command prompt
        pause
        exit /b 1
    )
)

echo %YELLOW%Using port: %PORT%%NC%
echo.

REM Try to detect if port exists
mode %PORT% >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo %YELLOW%Warning: Cannot verify port %PORT%%NC%
    echo.
    echo Available COM ports:
    wmic path Win32_SerialPort get DeviceID,Description 2>nul
    echo.
    echo If %PORT% is not correct, specify the port:
    echo   Usage: %0 COM4
    echo.
)

echo %GREEN%Step 1: Setting target to ESP32-C6%NC%
idf.py set-target esp32c6
if %ERRORLEVEL% NEQ 0 (
    echo %RED%Failed to set target%NC%
    pause
    exit /b 1
)

echo.
echo %GREEN%Step 2: Building firmware%NC%
idf.py build
if %ERRORLEVEL% NEQ 0 (
    echo %RED%Build failed!%NC%
    pause
    exit /b 1
)

echo.
echo %GREEN%Step 3: Flashing firmware%NC%
echo If this fails, try:
echo   1. Hold BOOT button
echo   2. Press RESET button
echo   3. Release BOOT button
echo.

idf.py -p %PORT% flash
if %ERRORLEVEL% NEQ 0 (
    echo %RED%Flash failed!%NC%
    echo.
    echo Troubleshooting:
    echo   - Check USB cable is data-capable ^(not charge-only^)
    echo   - Check correct COM port in Device Manager
    echo   - Install USB drivers if needed
    echo   - Try manual boot mode ^(see above^)
    echo.
    echo Retry with different port:
    echo   %0 COM4
    pause
    exit /b 1
)

echo.
echo %GREEN%Step 4: Opening serial monitor%NC%
echo Press Ctrl+] to exit monitor
echo.

idf.py -p %PORT% monitor

echo.
echo %GREEN%========================================%NC%
echo %GREEN%Flash complete! BTN-1 is ready.%NC%
echo %GREEN%========================================%NC%
echo.
echo Next steps:
echo   1. Hold Button 1 for 10 seconds to enter pairing mode
echo   2. Enable 'Permit Join' in Home Assistant ZHA
echo   3. Device will automatically join the network

pause