#include "ota_handler.h"
#include "version.h"
#include "power_management.h"
#include <string.h>
#include <esp_log.h>
#include <esp_flash.h>
#include <esp_app_format.h>
#include <esp_zigbee_core.h>

static const char *TAG = "OTA_HANDLER";

// Global OTA context
static ota_context_t g_ota_ctx = {0};
static ota_config_t g_ota_config = {
    .min_battery_percent = 20,
    .allow_downgrade = false,
    .chunk_size = 1024,
    .timeout_ms = 300000  // 5 minutes
};

// OTA status string conversion
const char* ota_status_to_string(ota_status_t status) {
    switch (status) {
        case OTA_STATUS_IDLE: return "IDLE";
        case OTA_STATUS_DOWNLOADING: return "DOWNLOADING";
        case OTA_STATUS_VERIFYING: return "VERIFYING";
        case OTA_STATUS_APPLYING: return "APPLYING";
        case OTA_STATUS_SUCCESS: return "SUCCESS";
        case OTA_STATUS_ERROR_INVALID_IMAGE: return "ERROR_INVALID_IMAGE";
        case OTA_STATUS_ERROR_VERSION: return "ERROR_VERSION";
        case OTA_STATUS_ERROR_BATTERY_LOW: return "ERROR_BATTERY_LOW";
        case OTA_STATUS_ERROR_WRITE_FAILED: return "ERROR_WRITE_FAILED";
        case OTA_STATUS_ERROR_VERIFY_FAILED: return "ERROR_VERIFY_FAILED";
        default: return "UNKNOWN";
    }
}

// Initialize OTA handler
esp_err_t ota_handler_init(const ota_config_t *config) {
    if (config) {
        memcpy(&g_ota_config, config, sizeof(ota_config_t));
    }
    
    // Clear OTA context
    memset(&g_ota_ctx, 0, sizeof(ota_context_t));
    g_ota_ctx.status = OTA_STATUS_IDLE;
    
    // Log current firmware version
    const esp_app_desc_t *app_desc = esp_app_get_description();
    ESP_LOGI(TAG, "Current firmware version: %s", get_firmware_version_string());
    ESP_LOGI(TAG, "App version: %s", app_desc->version);
    ESP_LOGI(TAG, "Build timestamp: %s", BUILD_TIMESTAMP);
    
    // Check and mark current app as valid if needed
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            ESP_LOGI(TAG, "Marking app as valid...");
            esp_ota_mark_app_valid_cancel_rollback();
        }
    }
    
    return ESP_OK;
}

// Deinitialize OTA handler
esp_err_t ota_handler_deinit(void) {
    if (g_ota_ctx.in_progress) {
        ota_abort_update();
    }
    return ESP_OK;
}

// Check battery level for OTA
bool ota_check_battery_level(void) {
    battery_info_t battery_info = {0};
    if (power_get_battery_info(&battery_info) == ESP_OK) {
        if (!battery_info.is_usb_connected && battery_info.percentage < g_ota_config.min_battery_percent) {
            ESP_LOGW(TAG, "Battery level too low for OTA: %d%% (required: %d%%)", 
                     battery_info.percentage, g_ota_config.min_battery_percent);
            return false;
        }
    }
    return true;
}

// Validate firmware version
bool ota_validate_version(uint32_t new_version) {
    uint32_t current_version = get_firmware_version();
    
    if (!g_ota_config.allow_downgrade && new_version < current_version) {
        ESP_LOGW(TAG, "Firmware downgrade not allowed. Current: 0x%08lx, New: 0x%08lx", 
                 current_version, new_version);
        return false;
    }
    
    if (new_version == current_version) {
        ESP_LOGW(TAG, "New firmware has same version as current: 0x%08lx", current_version);
        return false;
    }
    
    return true;
}

// Begin OTA update
esp_err_t ota_begin_update(uint32_t image_size, uint32_t version) {
    esp_err_t ret;
    
    // Check if update is already in progress
    if (g_ota_ctx.in_progress) {
        ESP_LOGE(TAG, "OTA update already in progress");
        return ESP_ERR_INVALID_STATE;
    }
    
    // Check battery level
    if (!ota_check_battery_level()) {
        g_ota_ctx.status = OTA_STATUS_ERROR_BATTERY_LOW;
        if (g_ota_ctx.status_cb) {
            g_ota_ctx.status_cb(g_ota_ctx.status);
        }
        return ESP_ERR_INVALID_STATE;
    }
    
    // Validate version
    if (!ota_validate_version(version)) {
        g_ota_ctx.status = OTA_STATUS_ERROR_VERSION;
        if (g_ota_ctx.status_cb) {
            g_ota_ctx.status_cb(g_ota_ctx.status);
        }
        return ESP_ERR_INVALID_VERSION;
    }
    
    // Get next update partition
    g_ota_ctx.update_partition = esp_ota_get_next_update_partition(NULL);
    if (!g_ota_ctx.update_partition) {
        ESP_LOGE(TAG, "Failed to get update partition");
        return ESP_ERR_NOT_FOUND;
    }
    
    ESP_LOGI(TAG, "Starting OTA update to partition %s, size: %lu bytes", 
             g_ota_ctx.update_partition->label, image_size);
    
    // Begin OTA
    ret = esp_ota_begin(g_ota_ctx.update_partition, image_size, &g_ota_ctx.handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(ret));
        g_ota_ctx.status = OTA_STATUS_ERROR_WRITE_FAILED;
        if (g_ota_ctx.status_cb) {
            g_ota_ctx.status_cb(g_ota_ctx.status);
        }
        return ret;
    }
    
    // Update context
    g_ota_ctx.image_size = image_size;
    g_ota_ctx.bytes_written = 0;
    g_ota_ctx.version = version;
    g_ota_ctx.status = OTA_STATUS_DOWNLOADING;
    g_ota_ctx.in_progress = true;
    
    if (g_ota_ctx.status_cb) {
        g_ota_ctx.status_cb(g_ota_ctx.status);
    }
    
    return ESP_OK;
}

// Write OTA data chunk
esp_err_t ota_write_chunk(const uint8_t *data, size_t size) {
    esp_err_t ret;
    
    if (!g_ota_ctx.in_progress || !data || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Write data to partition
    ret = esp_ota_write(g_ota_ctx.handle, data, size);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(ret));
        g_ota_ctx.status = OTA_STATUS_ERROR_WRITE_FAILED;
        if (g_ota_ctx.status_cb) {
            g_ota_ctx.status_cb(g_ota_ctx.status);
        }
        ota_abort_update();
        return ret;
    }
    
    // Update progress
    g_ota_ctx.bytes_written += size;
    uint8_t percent = (g_ota_ctx.bytes_written * 100) / g_ota_ctx.image_size;
    
    if (g_ota_ctx.progress_cb) {
        g_ota_ctx.progress_cb(g_ota_ctx.bytes_written, g_ota_ctx.image_size, percent);
    }
    
    ESP_LOGD(TAG, "OTA progress: %lu/%lu bytes (%d%%)", 
             g_ota_ctx.bytes_written, g_ota_ctx.image_size, percent);
    
    return ESP_OK;
}

// Verify OTA image
bool ota_verify_image(const esp_partition_t *partition) {
    esp_app_desc_t app_desc;
    esp_err_t ret;
    
    if (!partition) {
        return false;
    }
    
    // Read app description from partition
    ret = esp_ota_get_partition_description(partition, &app_desc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get partition description: %s", esp_err_to_name(ret));
        return false;
    }
    
    ESP_LOGI(TAG, "New firmware version: %s", app_desc.version);
    ESP_LOGI(TAG, "New firmware compile time: %s %s", app_desc.date, app_desc.time);
    
    // Verify magic word
    if (app_desc.magic_word != ESP_APP_DESC_MAGIC_WORD) {
        ESP_LOGE(TAG, "Invalid app magic word");
        return false;
    }
    
    return true;
}

// End OTA update
esp_err_t ota_end_update(void) {
    esp_err_t ret;
    
    if (!g_ota_ctx.in_progress) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // Check if all data was received
    if (g_ota_ctx.bytes_written != g_ota_ctx.image_size) {
        ESP_LOGE(TAG, "Incomplete OTA data: %lu/%lu bytes", 
                 g_ota_ctx.bytes_written, g_ota_ctx.image_size);
        ota_abort_update();
        return ESP_ERR_INVALID_SIZE;
    }
    
    // Set status to verifying
    g_ota_ctx.status = OTA_STATUS_VERIFYING;
    if (g_ota_ctx.status_cb) {
        g_ota_ctx.status_cb(g_ota_ctx.status);
    }
    
    // End OTA and verify
    ret = esp_ota_end(g_ota_ctx.handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed: %s", esp_err_to_name(ret));
        g_ota_ctx.status = OTA_STATUS_ERROR_VERIFY_FAILED;
        if (g_ota_ctx.status_cb) {
            g_ota_ctx.status_cb(g_ota_ctx.status);
        }
        g_ota_ctx.in_progress = false;
        return ret;
    }
    
    // Verify the new image
    if (!ota_verify_image(g_ota_ctx.update_partition)) {
        ESP_LOGE(TAG, "Image verification failed");
        g_ota_ctx.status = OTA_STATUS_ERROR_VERIFY_FAILED;
        if (g_ota_ctx.status_cb) {
            g_ota_ctx.status_cb(g_ota_ctx.status);
        }
        g_ota_ctx.in_progress = false;
        return ESP_ERR_OTA_VALIDATE_FAILED;
    }
    
    // Set boot partition
    g_ota_ctx.status = OTA_STATUS_APPLYING;
    if (g_ota_ctx.status_cb) {
        g_ota_ctx.status_cb(g_ota_ctx.status);
    }
    
    ret = esp_ota_set_boot_partition(g_ota_ctx.update_partition);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(ret));
        g_ota_ctx.status = OTA_STATUS_ERROR_WRITE_FAILED;
        if (g_ota_ctx.status_cb) {
            g_ota_ctx.status_cb(g_ota_ctx.status);
        }
        g_ota_ctx.in_progress = false;
        return ret;
    }
    
    // Success
    g_ota_ctx.status = OTA_STATUS_SUCCESS;
    if (g_ota_ctx.status_cb) {
        g_ota_ctx.status_cb(g_ota_ctx.status);
    }
    g_ota_ctx.in_progress = false;
    
    ESP_LOGI(TAG, "OTA update successful. Restart to boot new firmware.");
    
    return ESP_OK;
}

// Abort OTA update
esp_err_t ota_abort_update(void) {
    if (!g_ota_ctx.in_progress) {
        return ESP_OK;
    }
    
    if (g_ota_ctx.handle) {
        esp_ota_abort(g_ota_ctx.handle);
    }
    
    g_ota_ctx.status = OTA_STATUS_IDLE;
    g_ota_ctx.in_progress = false;
    
    ESP_LOGI(TAG, "OTA update aborted");
    
    return ESP_OK;
}

// Get OTA status
ota_status_t ota_get_status(void) {
    return g_ota_ctx.status;
}

// Get OTA progress
uint8_t ota_get_progress(void) {
    if (!g_ota_ctx.in_progress || g_ota_ctx.image_size == 0) {
        return 0;
    }
    return (g_ota_ctx.bytes_written * 100) / g_ota_ctx.image_size;
}

// Set progress callback
void ota_set_progress_callback(ota_progress_cb_t callback) {
    g_ota_ctx.progress_cb = callback;
}

// Set status callback
void ota_set_status_callback(ota_status_cb_t callback) {
    g_ota_ctx.status_cb = callback;
}

// Mark app as valid
esp_err_t ota_mark_app_valid(void) {
    return esp_ota_mark_app_valid_cancel_rollback();
}

// Rollback to previous firmware
esp_err_t ota_rollback(void) {
    ESP_LOGW(TAG, "Rolling back to previous firmware...");
    return esp_ota_mark_app_invalid_rollback_and_reboot();
}

// Check if update is pending
bool ota_is_update_pending(void) {
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        return (ota_state == ESP_OTA_IMG_PENDING_VERIFY);
    }
    
    return false;
}

// Zigbee OTA cluster initialization
esp_err_t ota_zigbee_init_cluster(uint8_t endpoint) {
    esp_zb_attribute_list_t *ota_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE);
    
    // Add OTA attributes
    uint32_t file_version = get_firmware_version();
    esp_zb_cluster_add_attr(ota_cluster, ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
                           ESP_ZB_ZCL_ATTR_OTA_UPGRADE_FILE_VERSION_ID,
                           ESP_ZB_ZCL_ATTR_TYPE_U32, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
                           &file_version);
    
    uint16_t manufacturer_id = MANUFACTURER_ID;
    esp_zb_cluster_add_attr(ota_cluster, ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
                           ESP_ZB_ZCL_ATTR_OTA_UPGRADE_MANUFACTURE_ID,
                           ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
                           &manufacturer_id);
    
    uint16_t image_type = IMAGE_TYPE;
    esp_zb_cluster_add_attr(ota_cluster, ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
                           ESP_ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_TYPE_ID,
                           ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
                           &image_type);
    
    uint8_t ota_status = 0; // Normal
    esp_zb_cluster_add_attr(ota_cluster, ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
                           ESP_ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_STATUS_ID,
                           ESP_ZB_ZCL_ATTR_TYPE_U8, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
                           &ota_status);
    
    uint32_t downloaded_file_version = 0xFFFFFFFF; // Invalid
    esp_zb_cluster_add_attr(ota_cluster, ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
                           ESP_ZB_ZCL_ATTR_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_ID,
                           ESP_ZB_ZCL_ATTR_TYPE_U32, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
                           &downloaded_file_version);
    
    ESP_LOGI(TAG, "OTA cluster initialized with version 0x%08lx", file_version);
    
    return ESP_OK;
}

// Zigbee OTA handlers (simplified for now - can be expanded based on coordinator requirements)
void ota_zigbee_image_notify_handler(const esp_zb_zcl_ota_upgrade_image_notify_message_t *message) {
    ESP_LOGI(TAG, "Received OTA image notify");
    // Send query next image request if needed
}

void ota_zigbee_query_next_image_handler(const esp_zb_zcl_ota_upgrade_query_next_image_message_t *message) {
    ESP_LOGI(TAG, "Received OTA query next image");
    // Respond with current image info
}

void ota_zigbee_image_block_handler(const esp_zb_zcl_ota_upgrade_image_block_message_t *message) {
    ESP_LOGI(TAG, "Received OTA image block request");
    // Handle image block request
}

void ota_zigbee_upgrade_end_handler(const esp_zb_zcl_ota_upgrade_end_message_t *message) {
    ESP_LOGI(TAG, "Received OTA upgrade end");
    // Handle upgrade end
}