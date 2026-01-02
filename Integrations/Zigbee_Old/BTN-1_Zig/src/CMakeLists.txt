# Get build time
string(TIMESTAMP BUILD_TIME_UTC "%Y-%m-%d %H:%M:%S" UTC)

# Component configuration
idf_component_register(
    SRCS
        "main.c"
        "button_handler.c"
        "zigbee_device.c"
        "power_management.c"
        "ota_handler.c"
        "version.c"
    
    INCLUDE_DIRS
        "."
    
    REQUIRES
        esp-zigbee-lib
        freertos
        nvs_flash
        driver
        esp_timer
        esp_system
        log
        app_update
    
    PRIV_REQUIRES
        heap
        esp_adc
)

# Component configuration options
idf_build_set_property(COMPILE_OPTIONS "-Wno-format" APPEND)

# Define component-specific compiler flags
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-variable -Wno-unused-function)

# Add component-specific definitions
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    BTN1_FIRMWARE_VERSION="1.0.0"
    BTN1_BUILD_TIME="${BUILD_TIME_UTC}"
)