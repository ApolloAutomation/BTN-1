/*
 * BTN-1 Zigbee Device Implementation
 * Device-specific Zigbee configuration and endpoint setup
 */

#include <string.h>
#include "esp_log.h"
#include "esp_zigbee_core.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "zigbee_device.h"
#include "button_handler.h"
#include "power_management.h"
#include "ota_handler.h"
#include "version.h"

static const char *TAG = "BTN1_DEVICE";

/* NVS namespace for button configuration */
#define BTN1_NVS_NAMESPACE "btn1_config"

/* Binding table structure */
typedef struct {
    bool valid;
    uint16_t addr;
    uint8_t endpoint;
    uint8_t mode; /* 0: unicast, 1: group, 2: broadcast */
} btn1_binding_t;

/* Device state structure */
typedef struct {
    btn1_binding_t bindings[BTN1_ENDPOINT_COUNT];
    uint8_t dimming_level[BTN1_ENDPOINT_COUNT]; /* Current dimming level for each button */
    uint8_t dimming_step[BTN1_ENDPOINT_COUNT];  /* Track dimming steps (0-3 for 25/50/75/100%) */
    uint8_t scene_id[BTN1_ENDPOINT_COUNT];      /* Scene ID for each button */
    uint16_t group_id[BTN1_ENDPOINT_COUNT];     /* Group ID for each button */
} btn1_device_state_t;

/* Global device state */
static btn1_device_state_t g_device_state = {0};

/* NVS handle */
static nvs_handle_t g_nvs_handle = 0;

/* Initialize NVS and load configuration */
static void btn1_nvs_init(void)
{
    esp_err_t err = nvs_open(BTN1_NVS_NAMESPACE, NVS_READWRITE, &g_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
        return;
    }
    
    /* Load bindings from NVS */
    size_t size = sizeof(g_device_state.bindings);
    err = nvs_get_blob(g_nvs_handle, "bindings", g_device_state.bindings, &size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "No saved bindings found, using defaults");
        /* Initialize default bindings - button 4 as group 0 (all off) */
        g_device_state.bindings[3].valid = true;
        g_device_state.bindings[3].addr = 0x0000; /* Group 0 */
        g_device_state.bindings[3].mode = 1; /* Group mode */
        g_device_state.bindings[3].endpoint = 1;
    }
    
    /* Load dimming states */
    size = sizeof(g_device_state.dimming_level);
    nvs_get_blob(g_nvs_handle, "dimming", g_device_state.dimming_level, &size);
    
    /* Load scene associations */
    size = sizeof(g_device_state.scene_id);
    err = nvs_get_blob(g_nvs_handle, "scenes", g_device_state.scene_id, &size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        /* Default scene IDs */
        for (int i = 0; i < BTN1_ENDPOINT_COUNT; i++) {
            g_device_state.scene_id[i] = i + 1;
        }
    }
}

/* Save configuration to NVS */
static void btn1_nvs_save(void)
{
    if (!g_nvs_handle) return;
    
    nvs_set_blob(g_nvs_handle, "bindings", g_device_state.bindings, sizeof(g_device_state.bindings));
    nvs_set_blob(g_nvs_handle, "dimming", g_device_state.dimming_level, sizeof(g_device_state.dimming_level));
    nvs_set_blob(g_nvs_handle, "scenes", g_device_state.scene_id, sizeof(g_device_state.scene_id));
    nvs_commit(g_nvs_handle);
}

/* Get destination for button commands */
static void get_button_destination(uint8_t button, uint16_t *addr, uint8_t *endpoint, uint8_t *mode)
{
    if (button < 1 || button > BTN1_ENDPOINT_COUNT) {
        *addr = 0x0000; /* Default to coordinator */
        *endpoint = 1;
        *mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
        return;
    }
    
    btn1_binding_t *binding = &g_device_state.bindings[button - 1];
    
    if (binding->valid) {
        *addr = binding->addr;
        *endpoint = binding->endpoint;
        
        switch (binding->mode) {
        case 1: /* Group */
            *mode = 0x01;  /* Group addressing mode - 16-bit address without endpoint */
            break;
        case 2: /* Broadcast */
            *mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
            *addr = 0xFFFF;
            break;
        default: /* Unicast */
            *mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
            break;
        }
    } else {
        /* No binding - use broadcast */
        *addr = 0xFFFF;
        *endpoint = 0xFF;
        *mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    }
}

/* Zigbee command sending utilities */
static void send_on_off_toggle(uint8_t endpoint)
{
    uint16_t dst_addr;
    uint8_t dst_endpoint;
    uint8_t addr_mode;
    
    get_button_destination(endpoint, &dst_addr, &dst_endpoint, &addr_mode);
    
    ESP_LOGI(TAG, "Sending ON/OFF toggle from endpoint %d to addr 0x%04x ep %d",
             endpoint, dst_addr, dst_endpoint);
    
    /* Acquire wake lock for Zigbee transmission */
    power_acquire_wake_lock(WAKE_LOCK_ZIGBEE, 5000);
    
    /* Create command structure */
    esp_zb_zcl_on_off_cmd_t cmd = {
        .zcl_basic_cmd = {
            .dst_addr_u = {
                .addr_short = dst_addr
            },
            .dst_endpoint = dst_endpoint,
            .src_endpoint = endpoint
        },
        .address_mode = addr_mode,
        .on_off_cmd_id = ESP_ZB_ZCL_CMD_ON_OFF_TOGGLE_ID
    };
    
    /* Send the command */
    esp_err_t err = esp_zb_zcl_on_off_cmd_req(&cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send ON/OFF toggle command: %s", esp_err_to_name(err));
    }
    
    /* Release wake lock after command is queued */
    power_release_wake_lock(WAKE_LOCK_ZIGBEE);
}

/* Send level control command */
static void send_level_control(uint8_t endpoint, uint8_t level)
{
    uint16_t dst_addr;
    uint8_t dst_endpoint;
    uint8_t addr_mode;
    
    get_button_destination(endpoint, &dst_addr, &dst_endpoint, &addr_mode);
    
    ESP_LOGI(TAG, "Sending level control from endpoint %d to addr 0x%04x ep %d, level %d%%",
             endpoint, dst_addr, dst_endpoint, (level * 100) / 254);
    
    /* Acquire wake lock for Zigbee transmission */
    power_acquire_wake_lock(WAKE_LOCK_ZIGBEE, 5000);
    
    /* Create command structure */
    esp_zb_zcl_move_to_level_cmd_t cmd = {
        .zcl_basic_cmd = {
            .dst_addr_u = {
                .addr_short = dst_addr
            },
            .dst_endpoint = dst_endpoint,
            .src_endpoint = endpoint
        },
        .address_mode = addr_mode,
        .level = level,
        .transition_time = 10  /* 1 second transition */
    };
    
    /* Send the command */
    esp_err_t err = esp_zb_zcl_level_move_to_level_cmd_req(&cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send level control command: %s", esp_err_to_name(err));
    }
    
    /* Save the level */
    g_device_state.dimming_level[endpoint - 1] = level;
    btn1_nvs_save();
    
    /* Release wake lock after command is queued */
    power_release_wake_lock(WAKE_LOCK_ZIGBEE);
}

/* Send scene recall command */
static void send_scene_recall(uint8_t endpoint, uint8_t scene_id)
{
    uint16_t dst_addr;
    uint8_t dst_endpoint;
    uint8_t addr_mode;
    
    get_button_destination(endpoint, &dst_addr, &dst_endpoint, &addr_mode);
    
    ESP_LOGI(TAG, "Sending scene recall from endpoint %d to addr 0x%04x ep %d, scene %d",
             endpoint, dst_addr, dst_endpoint, scene_id);
    
    /* Acquire wake lock for Zigbee transmission */
    power_acquire_wake_lock(WAKE_LOCK_ZIGBEE, 5000);
    
    /* Create command structure */
    esp_zb_zcl_scenes_recall_scene_cmd_t cmd = {
        .zcl_basic_cmd = {
            .dst_addr_u = {
                .addr_short = dst_addr
            },
            .dst_endpoint = dst_endpoint,
            .src_endpoint = endpoint
        },
        .address_mode = addr_mode,
        .group_id = 0,  /* Group 0 for now */
        .scene_id = scene_id
    };
    
    /* Send the command */
    esp_err_t err = esp_zb_zcl_scenes_recall_scene_cmd_req(&cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send scene recall command: %s", esp_err_to_name(err));
    }
    
    /* Release wake lock after command is queued */
    power_release_wake_lock(WAKE_LOCK_ZIGBEE);
}

/* Button event handler callback */
void btn1_button_event_handler(button_event_t *event)
{
    if (!event) {
        return;
    }
    
    ESP_LOGI(TAG, "Button %d event: %s", event->button_num,
             event->event == BUTTON_EVENT_SHORT_PRESS ? "SHORT_PRESS" :
             event->event == BUTTON_EVENT_LONG_PRESS ? "LONG_PRESS" :
             event->event == BUTTON_EVENT_DOUBLE_PRESS ? "DOUBLE_PRESS" : "UNKNOWN");
    
    /* Ensure button number is valid */
    if (event->button_num < 1 || event->button_num > BTN1_ENDPOINT_COUNT) {
        ESP_LOGE(TAG, "Invalid button number: %d", event->button_num);
        return;
    }
    
    uint8_t button_idx = event->button_num - 1;
    
    /* Map button events to Zigbee commands */
    switch (event->event) {
    case BUTTON_EVENT_SHORT_PRESS:
        /* Send ON/OFF toggle command */
        send_on_off_toggle(event->button_num);
        break;
        
    case BUTTON_EVENT_LONG_PRESS:
        /* Dimming control - cycle through 25%, 50%, 75%, 100% */
        g_device_state.dimming_step[button_idx] = (g_device_state.dimming_step[button_idx] + 1) % 4;
        uint8_t level = 0;
        switch (g_device_state.dimming_step[button_idx]) {
        case 0:
            level = 64;   /* 25% */
            break;
        case 1:
            level = 127;  /* 50% */
            break;
        case 2:
            level = 191;  /* 75% */
            break;
        case 3:
            level = 254;  /* 100% */
            break;
        }
        send_level_control(event->button_num, level);
        break;
        
    case BUTTON_EVENT_DOUBLE_PRESS:
        /* Scene recall - use scene ID associated with button */
        send_scene_recall(event->button_num, g_device_state.scene_id[button_idx]);
        break;
        
    default:
        break;
    }
}

/* Binding management functions */
esp_err_t btn1_add_binding(uint8_t button, uint16_t addr, uint8_t endpoint, uint8_t mode)
{
    if (button < 1 || button > BTN1_ENDPOINT_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    btn1_binding_t *binding = &g_device_state.bindings[button - 1];
    binding->valid = true;
    binding->addr = addr;
    binding->endpoint = endpoint;
    binding->mode = mode;
    
    btn1_nvs_save();
    
    ESP_LOGI(TAG, "Added binding for button %d: addr 0x%04x, ep %d, mode %d",
             button, addr, endpoint, mode);
    
    return ESP_OK;
}

esp_err_t btn1_remove_binding(uint8_t button)
{
    if (button < 1 || button > BTN1_ENDPOINT_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    g_device_state.bindings[button - 1].valid = false;
    btn1_nvs_save();
    
    ESP_LOGI(TAG, "Removed binding for button %d", button);
    
    return ESP_OK;
}

esp_err_t btn1_clear_all_bindings(void)
{
    memset(g_device_state.bindings, 0, sizeof(g_device_state.bindings));
    btn1_nvs_save();
    
    ESP_LOGI(TAG, "Cleared all bindings");
    
    return ESP_OK;
}

/* Group management functions */
esp_err_t btn1_set_group(uint8_t button, uint16_t group_id)
{
    if (button < 1 || button > BTN1_ENDPOINT_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    g_device_state.group_id[button - 1] = group_id;
    
    /* Update binding to use group mode */
    btn1_binding_t *binding = &g_device_state.bindings[button - 1];
    binding->valid = true;
    binding->addr = group_id;
    binding->endpoint = 1;  /* Not used for groups */
    binding->mode = 1;      /* Group mode */
    
    btn1_nvs_save();
    
    ESP_LOGI(TAG, "Set button %d to group %d", button, group_id);
    
    return ESP_OK;
}

/* Scene management functions */
esp_err_t btn1_set_scene(uint8_t button, uint8_t scene_id)
{
    if (button < 1 || button > BTN1_ENDPOINT_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    g_device_state.scene_id[button - 1] = scene_id;
    btn1_nvs_save();
    
    ESP_LOGI(TAG, "Set button %d to scene %d", button, scene_id);
    
    return ESP_OK;
}

/* Create basic cluster with device information */
static esp_zb_attribute_list_t *btn1_basic_cluster_create(void)
{
    esp_zb_basic_cluster_cfg_t basic_cfg = {
        .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,
        .power_source = 0x03, /* Battery */
    };
    
    esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(&basic_cfg);
    
    /* Add manufacturer name */
    esp_zb_basic_cluster_add_attr(basic_cluster, 
                                  ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,
                                  (void *)MANUFACTURER_NAME);
    
    /* Add model identifier */
    esp_zb_basic_cluster_add_attr(basic_cluster,
                                  ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,
                                  (void *)MODEL_IDENTIFIER);
    
    return basic_cluster;
}

/* Create identify cluster */
static esp_zb_attribute_list_t *btn1_identify_cluster_create(void)
{
    esp_zb_identify_cluster_cfg_t identify_cfg = {
        .identify_time = 0,
    };
    
    return esp_zb_identify_cluster_create(&identify_cfg);
}

/* Create on/off switch configuration cluster */
static esp_zb_attribute_list_t *btn1_on_off_switch_config_cluster_create(void)
{
    /* Create the cluster attribute list */
    esp_zb_attribute_list_t *switch_cfg_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG);
    
    /* Add switch type attribute (momentary switch) */
    uint8_t switch_type = ESP_ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_MOMENTARY;
    esp_zb_attribute_list_add_attr(switch_cfg_cluster,
                                   ESP_ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,
                                   0x0000, /* Switch Type attribute ID */
                                   ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
                                   ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
                                   &switch_type);
    
    /* Add switch actions attribute (all actions allowed) */
    uint8_t switch_actions = 0; /* 0 means all actions are allowed */
    esp_zb_attribute_list_add_attr(switch_cfg_cluster,
                                   ESP_ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,
                                   0x0010, /* Switch Actions attribute ID */
                                   ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
                                   ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE,
                                   &switch_actions);
    
    return switch_cfg_cluster;
}

/* Create cluster list for button endpoints */
static esp_zb_cluster_list_t *btn1_button_clusters_create(void)
{
    esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();
    
    /* Add server clusters */
    esp_zb_cluster_list_add_basic_cluster(cluster_list,
                                          btn1_basic_cluster_create(),
                                          ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    
    esp_zb_cluster_list_add_identify_cluster(cluster_list,
                                             btn1_identify_cluster_create(),
                                             ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    
    esp_zb_cluster_list_add_custom_cluster(cluster_list,
                                           btn1_on_off_switch_config_cluster_create(),
                                           ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    
    /* Add OTA upgrade server cluster for endpoint 1 only */
    static bool ota_initialized = false;
    if (!ota_initialized) {
        /* Create OTA cluster */
        esp_zb_attribute_list_t *ota_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE);
        
        /* Add OTA attributes */
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
        
        esp_zb_cluster_list_add_custom_cluster(cluster_list,
                                              ota_cluster,
                                              ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
        
        ota_initialized = true;
        ESP_LOGI(TAG, "OTA cluster initialized with version 0x%08lx", file_version);
    }
    
    /* Add client clusters - these allow the button to control other devices */
    esp_zb_attribute_list_t *on_off_client = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ON_OFF);
    esp_zb_cluster_list_add_on_off_cluster(cluster_list,
                                           on_off_client,
                                           ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
    
    esp_zb_attribute_list_t *level_control_client = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL);
    esp_zb_cluster_list_add_level_cluster(cluster_list,
                                          level_control_client,
                                          ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
    
    esp_zb_attribute_list_t *scenes_client = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_SCENES);
    esp_zb_cluster_list_add_scenes_cluster(cluster_list,
                                           scenes_client,
                                           ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
    
    esp_zb_attribute_list_t *groups_client = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_GROUPS);
    esp_zb_cluster_list_add_groups_cluster(cluster_list,
                                           groups_client,
                                           ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
    
    return cluster_list;
}

/* Create endpoint list with all button endpoints */
esp_zb_ep_list_t *btn1_ep_create(void)
{
    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_endpoint_config_t endpoint_config = {
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_ON_OFF_SWITCH_DEVICE_ID,
        .app_device_version = BTN1_DEVICE_VERSION,
    };
    
    /* Create endpoints for each button */
    for (uint8_t button = 1; button <= BTN1_ENDPOINT_COUNT; button++) {
        esp_zb_cluster_list_t *cluster_list = btn1_button_clusters_create();
        endpoint_config.endpoint = button;
        esp_zb_ep_list_add_ep(ep_list, cluster_list, endpoint_config);
        ESP_LOGI(TAG, "Created endpoint %d for button %d", button, button);
    }
    
    return ep_list;
}

/* Configure reporting for button events */
void btn1_configure_reporting(void)
{
    ESP_LOGI(TAG, "Configuring button reporting and loading saved configuration");
    
    /* Initialize NVS and load saved configuration */
    btn1_nvs_init();
    
    /* Log current configuration */
    for (int i = 0; i < BTN1_ENDPOINT_COUNT; i++) {
        if (g_device_state.bindings[i].valid) {
            ESP_LOGI(TAG, "Button %d: bound to addr 0x%04x, ep %d, mode %d",
                     i + 1, g_device_state.bindings[i].addr,
                     g_device_state.bindings[i].endpoint,
                     g_device_state.bindings[i].mode);
        } else {
            ESP_LOGI(TAG, "Button %d: no binding (will use broadcast)", i + 1);
        }
    }
}

/* Initialize BTN-1 Zigbee device */
void btn1_zigbee_init(void)
{
    /* Initialize device state with defaults */
    memset(&g_device_state, 0, sizeof(g_device_state));
    
    /* Set default scene IDs */
    for (int i = 0; i < BTN1_ENDPOINT_COUNT; i++) {
        g_device_state.scene_id[i] = i + 1;
    }
    
    ESP_LOGI(TAG, "BTN-1 Zigbee device initialized");
}