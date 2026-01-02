#ifndef OTA_HANDLER_H
#define OTA_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_ota_ops.h"
#include "esp_zigbee_core.h"

/* Forward declare the OTA message types if not already defined */
#ifndef ESP_ZB_ZCL_OTA_UPGRADE_TYPES_DEFINED
#define ESP_ZB_ZCL_OTA_UPGRADE_TYPES_DEFINED

typedef struct {
    uint8_t payload_type;
    uint32_t query_jitter;
    uint16_t manufacturer_code;
    uint16_t image_type;
    uint32_t new_file_version;
} esp_zb_zcl_ota_upgrade_image_notify_message_t;

typedef struct {
    uint8_t field_control;
    uint16_t manufacturer_code;
    uint16_t image_type;
    uint32_t current_file_version;
    uint16_t hardware_version;
} esp_zb_zcl_ota_upgrade_query_next_image_message_t;

typedef struct {
    uint8_t field_control;
    uint16_t manufacturer_code;
    uint16_t image_type;
    uint32_t file_version;
    uint32_t file_offset;
    uint8_t max_data_size;
    uint64_t request_node_address;
} esp_zb_zcl_ota_upgrade_image_block_message_t;

typedef struct {
    esp_zb_zcl_status_t status;
    uint16_t manufacturer_code;
    uint16_t image_type;
    uint32_t file_version;
} esp_zb_zcl_ota_upgrade_end_message_t;

#endif /* ESP_ZB_ZCL_OTA_UPGRADE_TYPES_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif

// OTA status codes
typedef enum {
    OTA_STATUS_IDLE = 0,
    OTA_STATUS_DOWNLOADING,
    OTA_STATUS_VERIFYING,
    OTA_STATUS_APPLYING,
    OTA_STATUS_SUCCESS,
    OTA_STATUS_ERROR_INVALID_IMAGE,
    OTA_STATUS_ERROR_VERSION,
    OTA_STATUS_ERROR_BATTERY_LOW,
    OTA_STATUS_ERROR_WRITE_FAILED,
    OTA_STATUS_ERROR_VERIFY_FAILED
} ota_status_t;

// OTA configuration
typedef struct {
    uint8_t min_battery_percent;    // Minimum battery level required for OTA
    bool allow_downgrade;           // Allow firmware downgrade
    uint32_t chunk_size;            // Size of each OTA data chunk
    uint32_t timeout_ms;            // Timeout for OTA operations
} ota_config_t;

// OTA progress callback
typedef void (*ota_progress_cb_t)(uint32_t offset, uint32_t total_size, uint8_t percent);

// OTA status callback  
typedef void (*ota_status_cb_t)(ota_status_t status);

// OTA context structure
typedef struct {
    esp_ota_handle_t handle;
    const esp_partition_t *update_partition;
    uint32_t image_size;
    uint32_t bytes_written;
    uint32_t version;
    ota_status_t status;
    ota_progress_cb_t progress_cb;
    ota_status_cb_t status_cb;
    bool in_progress;
} ota_context_t;

// OTA handler functions
esp_err_t ota_handler_init(const ota_config_t *config);
esp_err_t ota_handler_deinit(void);

// OTA operation functions
esp_err_t ota_begin_update(uint32_t image_size, uint32_t version);
esp_err_t ota_write_chunk(const uint8_t *data, size_t size);
esp_err_t ota_end_update(void);
esp_err_t ota_abort_update(void);

// OTA verification and validation
bool ota_verify_image(const esp_partition_t *partition);
bool ota_validate_version(uint32_t new_version);
bool ota_check_battery_level(void);

// OTA status and progress
ota_status_t ota_get_status(void);
uint8_t ota_get_progress(void);
void ota_set_progress_callback(ota_progress_cb_t callback);
void ota_set_status_callback(ota_status_cb_t callback);

// Zigbee OTA cluster handlers
esp_err_t ota_zigbee_init_cluster(uint8_t endpoint);
void ota_zigbee_image_notify_handler(const esp_zb_zcl_ota_upgrade_image_notify_message_t *message);
void ota_zigbee_query_next_image_handler(const esp_zb_zcl_ota_upgrade_query_next_image_message_t *message);
void ota_zigbee_image_block_handler(const esp_zb_zcl_ota_upgrade_image_block_message_t *message);
void ota_zigbee_upgrade_end_handler(const esp_zb_zcl_ota_upgrade_end_message_t *message);

// Utility functions
const char* ota_status_to_string(ota_status_t status);
esp_err_t ota_mark_app_valid(void);
esp_err_t ota_rollback(void);
bool ota_is_update_pending(void);

#ifdef __cplusplus
}
#endif

#endif // OTA_HANDLER_H