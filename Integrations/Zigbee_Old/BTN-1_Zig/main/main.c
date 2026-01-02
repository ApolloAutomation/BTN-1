/* 
 * BTN-1 Zigbee Firmware
 * ESP-IDF based implementation for ESP32-C6
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "esp_zigbee_core.h"
#include "zigbee_device.h"
#include "button_handler.h"
#include "power_management.h"
#include "ota_handler.h"
#include "version.h"
#include "simple_led.h"  // Simple LED implementation

/* Define logging tag */
static const char *TAG = "BTN1_ZIGBEE";

/* Forward declarations */
static void ota_progress_callback(uint32_t offset, uint32_t total_size, uint8_t percent);
static void ota_status_callback(ota_status_t status);

/* Power statistics logging timer */
static void power_stats_timer_callback(void* arg)
{
    power_stats_t stats;
    if (power_get_stats(&stats) == ESP_OK) {
        uint32_t total_time = stats.total_active_time_ms + stats.total_sleep_time_ms;
        uint32_t sleep_percent = 0;
        if (total_time > 0) {
            sleep_percent = (stats.total_sleep_time_ms * 100) / total_time;
        }
        
        ESP_LOGI(TAG, "Power Stats: Active %lu ms, Sleep %lu ms (%lu%%), Wake count: %lu, Battery: %lu mV (%d%%)",
                 stats.total_active_time_ms, stats.total_sleep_time_ms, sleep_percent,
                 stats.wake_count, stats.battery_voltage_mv, stats.battery_percentage);
        
        /* Log OTA status if update is in progress */
        ota_status_t ota_status = ota_get_status();
        if (ota_status != OTA_STATUS_IDLE) {
            ESP_LOGI(TAG, "OTA Status: %s, Progress: %d%%",
                     ota_status_to_string(ota_status), ota_get_progress());
        }
    }
}

/* Zigbee configuration */
#define INSTALLCODE_POLICY_ENABLE    false
#define ESP_ZB_PRIMARY_CHANNEL_MASK  ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK  /* Allow all channels */

#define ESP_ZB_ZED_CONFIG()                                     \
    {                                                           \
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,                   \
        .install_code_policy = INSTALLCODE_POLICY_ENABLE,      \
        .nwk_cfg = {                                            \
            .zed_cfg = {                                        \
                .ed_timeout = ESP_ZB_ED_AGING_TIMEOUT_64MIN,   \
                .keep_alive = 3000,                             \
            },                                                  \
        },                                                      \
    }

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal)
{
    uint32_t *p_sg_p = signal->p_app_signal;
    esp_err_t err_status = signal->esp_err_status;
    esp_zb_app_signal_type_t sig_type = *p_sg_p;
    esp_zb_app_signal_msg_t *p_sg_msg = (esp_zb_app_signal_msg_t *)signal->p_app_signal;

    switch (sig_type) {
    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
        if (err_status == ESP_OK) {
            ESP_LOGI(TAG, "Device started up in %s factory-reset mode",
                     sig_type == ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START ? "" : "non");
            if (sig_type == ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START) {
                ESP_LOGI(TAG, "=================================================================");
                ESP_LOGI(TAG, "Device is not joined to a network. Manual pairing is required:");
                ESP_LOGI(TAG, "1. Press and hold Button 1 for 10 seconds to enter pairing mode");
                ESP_LOGI(TAG, "2. Enable 'Permit Join' in Home Assistant ZHA integration");
                ESP_LOGI(TAG, "3. The device will automatically join the network");
                ESP_LOGI(TAG, "=================================================================");
                /* Set network state to not joined */
                zigbee_set_network_joined(false);
                /* Do not automatically start network steering - wait for manual trigger */
            } else {
                /* Device rebooted, check if we're in a network */
                if (esp_zb_get_pan_id() != 0xFFFF) {
                    ESP_LOGI(TAG, "Device is already joined to a network (PAN ID: 0x%04hx)",
                             esp_zb_get_pan_id());
                    zigbee_set_network_joined(true);
                } else {
                    ESP_LOGI(TAG, "Device is not in a network. Use Button 1 long press to enter pairing mode");
                    zigbee_set_network_joined(false);
                }
            }
        } else {
            /* commissioning failed */
            ESP_LOGW(TAG, "Failed to initialize Zigbee stack (status: %d)", err_status);
        }
        break;
        
    case ESP_ZB_BDB_SIGNAL_STEERING:
        if (err_status == ESP_OK) {
            esp_zb_ieee_addr_t extended_pan_id;
            esp_zb_get_extended_pan_id(extended_pan_id);
            ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d)",
                     extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
                     extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
                     esp_zb_get_pan_id(), esp_zb_get_current_channel());
            /* Set network state to joined - this will trigger LED connected indication */
            zigbee_set_network_joined(true);
            /* Configure reporting after joining network */
            btn1_configure_reporting();
        } else {
            ESP_LOGW(TAG, "Network steering failed (status: %s)", esp_err_to_name(err_status));
            ESP_LOGI(TAG, "Failed to join network. Please ensure:");
            ESP_LOGI(TAG, "- Home Assistant ZHA 'Permit Join' is enabled");
            ESP_LOGI(TAG, "- The device is within range of the coordinator");
            ESP_LOGI(TAG, "Press Button 1 for 10 seconds to retry pairing");
            zigbee_set_network_joined(false);
            /* Show error indication for failed joining */
            // led_indicate_error(); // Temporarily disabled - LED implementation needs update
            /* Do not automatically retry - wait for manual trigger */
        }
        break;
        
    case ESP_ZB_COMMON_SIGNAL_CAN_SLEEP:
        ESP_LOGI(TAG, "Device can enter sleep mode");
        /* Notify power management that Zigbee sleep is allowed */
        power_on_zigbee_sleep_allowed();
        break;
        
    default:
        ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s",
                 esp_zb_zdo_signal_to_string(sig_type), sig_type,
                 esp_err_to_name(err_status));
        break;
    }
}

static void esp_zb_task(void *pvParameters)
{
    /* Initialize Zigbee stack */
    esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZED_CONFIG();
    esp_zb_init(&zb_nwk_cfg);
    
    /* Create endpoints for the device */
    esp_zb_ep_list_t *ep_list = btn1_ep_create();
    if (!ep_list) {
        ESP_LOGE(TAG, "Failed to create endpoint list");
        vTaskDelete(NULL);
        return;
    }
    
    /* Register the device */
    esp_zb_device_register(ep_list);
    
    /* Set the device configuration */
    esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);
    esp_zb_set_rx_on_when_idle(false);  /* Save power as an end device */
    
    /* Enable Zigbee sleep for power saving */
    esp_zb_sleep_enable(true);
    
    /* Start Zigbee stack main loop */
    ESP_ERROR_CHECK(esp_zb_start(false));
    /* The Zigbee stack task will handle the main loop internally */
    vTaskDelete(NULL);
}

/* OTA progress callback */
static void ota_progress_callback(uint32_t offset, uint32_t total_size, uint8_t percent)
{
    ESP_LOGI(TAG, "OTA Progress: %lu/%lu bytes (%d%%)", offset, total_size, percent);
}

/* OTA status callback */
static void ota_status_callback(ota_status_t status)
{
    ESP_LOGI(TAG, "OTA Status Changed: %s", ota_status_to_string(status));
    
    if (status == OTA_STATUS_SUCCESS) {
        ESP_LOGI(TAG, "OTA update completed successfully. Rebooting in 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        esp_restart();
    }
}

void app_main(void)
{
    /* Initialize NVS first */
    ESP_ERROR_CHECK(nvs_flash_init());
    
    /* Configure Zigbee platform - no longer using deprecated macros */
    esp_zb_platform_config_t config = {
        .radio_config = {
            .radio_mode = ZB_RADIO_MODE_NATIVE,
        },
        .host_config = {
            .host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE,
        }
    };
    
    /* Display firmware information */
    ESP_LOGI(TAG, "=== BTN-1 Zigbee Firmware ===");
    ESP_LOGI(TAG, "Version: %s", get_firmware_version_string());
    ESP_LOGI(TAG, "Build: %s", BUILD_TIMESTAMP);
    ESP_LOGI(TAG, "=============================");
    
    /* Check if we're running after an OTA update */
    if (ota_is_update_pending()) {
        ESP_LOGI(TAG, "Running new firmware after OTA update");
        /* Perform any validation here if needed */
        /* If everything is OK, mark as valid */
        ESP_ERROR_CHECK(ota_mark_app_valid());
        ESP_LOGI(TAG, "New firmware validated successfully");
    }
    
    /* Initialize power management */
    power_config_t pm_config = {
        .activity_timeout_ms = 5000,    /* 5 seconds before sleep */
        .min_sleep_duration_ms = 100,
        .adaptive_sleep_enabled = true,
        .battery_monitoring_enabled = false  /* Enable if battery ADC connected */
    };
    esp_err_t pm_err = power_management_init(&pm_config);
    if (pm_err != ESP_OK) {
        ESP_LOGW(TAG, "Power management initialization failed: %s", esp_err_to_name(pm_err));
        ESP_LOGW(TAG, "Continuing without power management features");
        /* Continue without power management - device will work but won't sleep */
    } else {
        ESP_LOGI(TAG, "Power management initialized successfully");
    }
    
    /* Enable power management debug logging in development */
    #ifdef CONFIG_APP_DEBUG_MODE
    power_enable_debug_logging(true);
    #endif
    
    /* Initialize LED indicator system */
    // Initialize simple LED indicator
    simple_led_init();
    ESP_LOGI(TAG, "Simple LED indicator system initialized");
    
    // Brief startup flash
    simple_led_indicate_button_press();
    
    /* Show startup indication */
    // led_indicate_startup();
    
    /* Initialize Zigbee platform */
    ESP_ERROR_CHECK(esp_zb_platform_config(&config));
    
    /* Initialize BTN-1 Zigbee device */
    btn1_zigbee_init();
    
    /* Initialize button hardware */
    ESP_ERROR_CHECK(button_handler_init());
    
    /* Register button event callback */
    ESP_ERROR_CHECK(button_register_callback(btn1_button_event_handler));
    
    /* Start button handler task */
    ESP_ERROR_CHECK(button_handler_start());
    
    ESP_LOGI(TAG, "Button handler initialized and started");
    
    /* Display button instructions */
    ESP_LOGI(TAG, "=== Button Controls ===");
    ESP_LOGI(TAG, "Button 1 Long Press (10s): Enter pairing mode");
    ESP_LOGI(TAG, "All 4 Buttons (5s): Factory reset");
    ESP_LOGI(TAG, "Short Press: Toggle lights on/off");
    ESP_LOGI(TAG, "Long Press (0.5s): Adjust brightness");
    ESP_LOGI(TAG, "Double Press: Recall scene");
    ESP_LOGI(TAG, "=======================");
    
    /* Initialize OTA handler */
    ota_config_t ota_config = {
        .min_battery_percent = 20,
        .allow_downgrade = false,
        .chunk_size = 1024,
        .timeout_ms = 300000
    };
    ESP_ERROR_CHECK(ota_handler_init(&ota_config));
    
    /* Set OTA callbacks */
    ota_set_progress_callback(ota_progress_callback);
    ota_set_status_callback(ota_status_callback);
    
    /* Create Zigbee task */
    xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);
    
    #ifdef CONFIG_APP_DEBUG_MODE
    /* Start power statistics logging timer in debug mode */
    esp_timer_handle_t stats_timer;
    const esp_timer_create_args_t stats_timer_args = {
        .callback = power_stats_timer_callback,
        .name = "power_stats"
    };
    esp_timer_create(&stats_timer_args, &stats_timer);
    esp_timer_start_periodic(stats_timer, 30000000);  /* Log every 30 seconds */
    #endif
}