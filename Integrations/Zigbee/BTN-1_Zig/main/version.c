#include "version.h"

const char* get_firmware_version_string(void) {
    return FIRMWARE_VERSION_STRING;
}

uint32_t get_firmware_version(void) {
    return FIRMWARE_VERSION;
}

int compare_versions(uint32_t version1, uint32_t version2) {
    if (version1 > version2) return 1;
    if (version1 < version2) return -1;
    return 0;
}