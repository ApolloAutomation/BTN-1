/*
 * Apollo BTN-1 Zigbee Firmware
 *
 * 4-button macro deck with Zigbee connectivity.
 * Each button is exposed as a separate On/Off endpoint.
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_zigbee_core.h"
#include "platform/esp_zigbee_platform.h"
#include "led_strip.h"

static const char *TAG = "BTN1_ZB";

/* BTN-1 Hardware Pins */
#define BUTTON_1_PIN          GPIO_NUM_2
#define BUTTON_2_PIN          GPIO_NUM_4
#define BUTTON_3_PIN          GPIO_NUM_6
#define BUTTON_4_PIN          GPIO_NUM_5
#define RESET_BUTTON_PIN      GPIO_NUM_9
#define LED_STRIP_PIN         GPIO_NUM_21
#define ACCESSORY_POWER_PIN   GPIO_NUM_18

#define NUM_LEDS              4
#define NUM_BUTTONS           4

/* Zigbee Endpoints */
#define BUTTON_1_ENDPOINT     10
#define BUTTON_2_ENDPOINT     11
#define BUTTON_3_ENDPOINT     12
#define BUTTON_4_ENDPOINT     13

#define MANUFACTURER_NAME     "\x06""Apollo"
#define MODEL_ID              "\x05""BTN-1"

/* Model IDs for each button (Zigbee string format: length byte + string) */
static const char *BUTTON_MODEL_IDS[NUM_BUTTONS] = {
    "\x0e""BTN-1 Button 1",
    "\x0e""BTN-1 Button 2",
    "\x0e""BTN-1 Button 3",
    "\x0e""BTN-1 Button 4",
};

/* Zigbee Config */
#define ESP_ZB_ZED_CONFIG() {                                   \
    .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,                       \
    .install_code_policy = false,                               \
    .nwk_cfg.zed_cfg = {                                        \
        .ed_timeout = ESP_ZB_ED_AGING_TIMEOUT_64MIN,            \
        .keep_alive = 3000,                                     \
    },                                                          \
}

/* Button tracking */
typedef struct {
    gpio_num_t pin;
    uint8_t endpoint;
    bool state;
    uint32_t last_press;
} button_t;

static button_t buttons[NUM_BUTTONS] = {
    { BUTTON_1_PIN, BUTTON_1_ENDPOINT, false, 0 },
    { BUTTON_2_PIN, BUTTON_2_ENDPOINT, false, 0 },
    { BUTTON_3_PIN, BUTTON_3_ENDPOINT, false, 0 },
    { BUTTON_4_PIN, BUTTON_4_ENDPOINT, false, 0 },
};

static led_strip_handle_t led_strip;
static bool zigbee_connected = false;
static QueueHandle_t button_queue = NULL;

/********************* LED Functions *********************/

static void led_init(void)
{
    led_strip_config_t config = {
        .strip_gpio_num = LED_STRIP_PIN,
        .max_leds = NUM_LEDS,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&config, &rmt_config, &led_strip));
    led_strip_clear(led_strip);
}

static void led_set(uint8_t idx, uint8_t r, uint8_t g, uint8_t b)
{
    if (idx < NUM_LEDS) {
        led_strip_set_pixel(led_strip, idx, r, g, b);
        led_strip_refresh(led_strip);
    }
}

static void led_set_all(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        led_strip_set_pixel(led_strip, i, r, g, b);
    }
    led_strip_refresh(led_strip);
}

static void led_clear_all(void)
{
    led_strip_clear(led_strip);
}

static void led_update_button(uint8_t idx)
{
    if (idx < NUM_BUTTONS) {
        if (buttons[idx].state) {
            led_set(idx, 64, 64, 64);
        } else {
            led_set(idx, 0, 0, 0);
        }
    }
}

/********************* Button Functions *********************/

static void IRAM_ATTR button_isr(void *arg)
{
    uint32_t idx = (uint32_t)arg;
    xQueueSendFromISR(button_queue, &idx, NULL);
}

static void button_init(void)
{
    // Main buttons (active high with pulldown)
    gpio_config_t btn_cfg = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };

    for (int i = 0; i < NUM_BUTTONS; i++) {
        btn_cfg.pin_bit_mask = (1ULL << buttons[i].pin);
        gpio_config(&btn_cfg);
        gpio_isr_handler_add(buttons[i].pin, button_isr, (void *)(uintptr_t)i);
    }

    // Reset button (active low with pullup)
    gpio_config_t rst_cfg = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pin_bit_mask = (1ULL << RESET_BUTTON_PIN),
    };
    gpio_config(&rst_cfg);
}

static void button_toggle(uint8_t idx)
{
    if (idx >= NUM_BUTTONS) return;

    buttons[idx].state = !buttons[idx].state;
    ESP_LOGI(TAG, "Button %d -> %s", idx + 1, buttons[idx].state ? "ON" : "OFF");
    led_update_button(idx);

    if (zigbee_connected) {
        esp_zb_lock_acquire(portMAX_DELAY);

        esp_zb_zcl_set_attribute_val(
            buttons[idx].endpoint,
            ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
            ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
            ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
            &buttons[idx].state,
            false
        );

        esp_zb_zcl_report_attr_cmd_t cmd = {
            .zcl_basic_cmd.src_endpoint = buttons[idx].endpoint,
            .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
            .clusterID = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
            .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI,
        };
        esp_zb_zcl_report_attr_cmd_req(&cmd);

        esp_zb_lock_release();
    }
}

static void button_task(void *arg)
{
    uint32_t idx;
    uint32_t reset_start = 0;
    bool reset_held = false;

    while (1) {
        if (xQueueReceive(button_queue, &idx, pdMS_TO_TICKS(50))) {
            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            if ((now - buttons[idx].last_press) > 200) {
                buttons[idx].last_press = now;
                button_toggle(idx);
            }
        }

        // Factory reset check (3s hold)
        if (gpio_get_level(RESET_BUTTON_PIN) == 0) {
            if (!reset_held) {
                reset_held = true;
                reset_start = xTaskGetTickCount() * portTICK_PERIOD_MS;
            } else if ((xTaskGetTickCount() * portTICK_PERIOD_MS - reset_start) > 3000) {
                ESP_LOGW(TAG, "Factory reset!");
                led_set_all(255, 0, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
                esp_zb_factory_reset();
            }
        } else {
            reset_held = false;
        }
    }
}

/********************* Zigbee Functions *********************/

static esp_zb_cluster_list_t *create_cluster_list(int button_idx)
{
    esp_zb_cluster_list_t *list = esp_zb_zcl_cluster_list_create();

    // Basic cluster
    esp_zb_basic_cluster_cfg_t basic_cfg = {
        .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,
        .power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_BATTERY,
    };
    esp_zb_attribute_list_t *basic = esp_zb_basic_cluster_create(&basic_cfg);
    esp_zb_basic_cluster_add_attr(basic, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (void *)MANUFACTURER_NAME);
    esp_zb_basic_cluster_add_attr(basic, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (void *)BUTTON_MODEL_IDS[button_idx]);
    esp_zb_cluster_list_add_basic_cluster(list, basic, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    // Identify cluster
    esp_zb_identify_cluster_cfg_t id_cfg = { .identify_time = 0 };
    esp_zb_attribute_list_t *identify = esp_zb_identify_cluster_create(&id_cfg);
    esp_zb_cluster_list_add_identify_cluster(list, identify, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    // On/Off cluster
    esp_zb_on_off_cluster_cfg_t onoff_cfg = { .on_off = false };
    esp_zb_attribute_list_t *onoff = esp_zb_on_off_cluster_create(&onoff_cfg);
    esp_zb_cluster_list_add_on_off_cluster(list, onoff, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    return list;
}

static void create_endpoints(void)
{
    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();

    for (int i = 0; i < NUM_BUTTONS; i++) {
        esp_zb_cluster_list_t *clusters = create_cluster_list(i);
        esp_zb_endpoint_config_t cfg = {
            .endpoint = buttons[i].endpoint,
            .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
            .app_device_id = ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID,
            .app_device_version = 0,
        };
        esp_zb_ep_list_add_ep(ep_list, clusters, cfg);
        ESP_LOGI(TAG, "Endpoint %d created for button %d", buttons[i].endpoint, i + 1);
    }

    esp_zb_device_register(ep_list);
}

static esp_err_t zb_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *msg)
{
    if (!msg || msg->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_FAIL;

    if (msg->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF &&
        msg->attribute.id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID) {

        bool state = *(bool *)msg->attribute.data.value;
        for (int i = 0; i < NUM_BUTTONS; i++) {
            if (buttons[i].endpoint == msg->info.dst_endpoint) {
                buttons[i].state = state;
                led_update_button(i);
                ESP_LOGI(TAG, "Button %d set to %s via Zigbee", i + 1, state ? "ON" : "OFF");
                break;
            }
        }
    }
    return ESP_OK;
}

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t id, const void *msg)
{
    if (id == ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID) {
        return zb_attribute_handler((esp_zb_zcl_set_attr_value_message_t *)msg);
    }
    return ESP_OK;
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal)
{
    uint32_t *p_sg_p = signal->p_app_signal;
    esp_err_t err = signal->esp_err_status;
    esp_zb_app_signal_type_t sig = *p_sg_p;

    switch (sig) {
        case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
            ESP_LOGI(TAG, "Zigbee stack initialized");
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
            break;

        case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
            if (err == ESP_OK) {
                if (esp_zb_bdb_is_factory_new()) {
                    ESP_LOGI(TAG, "Factory new, searching for network...");
                    led_set_all(0, 0, 64);  // Blue
                    esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
                } else {
                    ESP_LOGI(TAG, "Rejoined network!");
                    zigbee_connected = true;
                    led_set_all(0, 64, 0);  // Green
                    vTaskDelay(pdMS_TO_TICKS(500));
                    led_clear_all();
                }
            } else {
                ESP_LOGW(TAG, "Init failed: %s", esp_err_to_name(err));
            }
            break;

        case ESP_ZB_BDB_SIGNAL_STEERING:
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Joined network!");
                zigbee_connected = true;
                led_set_all(0, 64, 0);  // Green
                vTaskDelay(pdMS_TO_TICKS(1000));
                led_clear_all();
            } else {
                ESP_LOGI(TAG, "Network steering failed, retrying...");
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
            }
            break;

        default:
            ESP_LOGD(TAG, "Signal: 0x%x, status: %s", sig, esp_err_to_name(err));
            break;
    }
}

static void zigbee_task(void *arg)
{
    esp_zb_cfg_t cfg = ESP_ZB_ZED_CONFIG();
    esp_zb_init(&cfg);

    create_endpoints();
    esp_zb_core_action_handler_register(zb_action_handler);
    esp_zb_set_primary_network_channel_set(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);

    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

/********************* Main *********************/

void app_main(void)
{
    ESP_LOGI(TAG, "=== Apollo BTN-1 Zigbee ===");

    // Enable accessory power
    gpio_config_t pwr = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << ACCESSORY_POWER_PIN),
    };
    gpio_config(&pwr);
    gpio_set_level(ACCESSORY_POWER_PIN, 1);
    ESP_LOGI(TAG, "Power enabled");

    // Init LEDs
    led_init();
    led_set_all(64, 64, 0);  // Yellow = starting
    ESP_LOGI(TAG, "LEDs initialized");

    // Init buttons
    gpio_install_isr_service(0);
    button_init();
    button_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(button_task, "buttons", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "Buttons initialized");

    // Init Zigbee
    esp_zb_platform_config_t zb_cfg = {
        .radio_config = {
            .radio_mode = ZB_RADIO_MODE_NATIVE,
        },
        .host_config = {
            .host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE,
        },
    };
    ESP_ERROR_CHECK(esp_zb_platform_config(&zb_cfg));
    xTaskCreate(zigbee_task, "zigbee", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "Zigbee started");
}
