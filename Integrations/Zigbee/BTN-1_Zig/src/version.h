#ifndef VERSION_H
#define VERSION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Firmware version information
#define FIRMWARE_VERSION_MAJOR      1
#define FIRMWARE_VERSION_MINOR      0
#define FIRMWARE_VERSION_PATCH      0
#define FIRMWARE_VERSION_BUILD      1

// Create a 32-bit version number for easy comparison
#define FIRMWARE_VERSION ((FIRMWARE_VERSION_MAJOR << 24) | \
                         (FIRMWARE_VERSION_MINOR << 16) | \
                         (FIRMWARE_VERSION_PATCH << 8) | \
                         (FIRMWARE_VERSION_BUILD))

// Version string format: "MAJOR.MINOR.PATCH.BUILD"
#define FIRMWARE_VERSION_STRING     "1.0.0.1"

// Manufacturer ID for OTA (Apollo Automation)
#define MANUFACTURER_ID             0x1234  // Replace with actual manufacturer ID

// Image type for BTN-1
#define IMAGE_TYPE                  0x0001

// Build timestamp (will be updated by build system)
#ifndef BUILD_TIMESTAMP
#define BUILD_TIMESTAMP             __DATE__ " " __TIME__
#endif

// Function prototypes
const char* get_firmware_version_string(void);
uint32_t get_firmware_version(void);
int compare_versions(uint32_t version1, uint32_t version2);

#ifdef __cplusplus
}
#endif

#endif // VERSION_H