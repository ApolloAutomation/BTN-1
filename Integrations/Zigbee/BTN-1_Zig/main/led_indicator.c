/*
 * BTN-1 LED Indicator Module Implementation
 * Uses WS2812 addressable RGB LED strip with 4 LEDs
 * Provides visual feedback for network and button operations
 */

#include "led_indicator.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"
#include <string.h>

static const char *TAG = "LED_INDICATOR";

/* LED strip configuration */
#define LED_STRIP_RESOLUTION_HZ  10000000 /* 10MHz resolution */
#define LED_STRIP_BYTES_PER_PIXEL 3       /* RGB, 3 bytes per pixel */

/* LED state management */
typedef struct {
    led_color_t colors[LED_COUNT];    /* Current color for each LED */
    rmt_channel_handle_t rmt_chan;    /* RMT channel handle */
    rmt_encoder_handle_t encoder;     /* LED strip encoder */
    led_pattern_t current_pattern;    /* Current active pattern */
    TimerHandle_t pattern_timer;       /* Timer for pattern updates */
    SemaphoreHandle_t mutex;           /* Mutex for thread safety */
    bool initialized;                  /* Initialization status */
    uint8_t pattern_state;            /* Pattern state machine */
    uint8_t pattern_count;            /* Pattern iteration counter */
} led_state_t;

static led_state_t led_state = {
    .initialized = false,
    .current_pattern = LED_PATTERN_OFF
};

/* Forward declarations */
static void pattern_timer_callback(TimerHandle_t xTimer);
static esp_err_t led_strip_init(void);
static esp_err_t led_strip_deinit(void);

/* LED strip encoder configuration for WS2812 */
typedef struct {
    uint32_t resolution;
    rmt_bytes_encoder_handle_t bytes_encoder;
    rmt_copy_encoder_handle_t copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} led_strip_encoder_t;

static esp_err_t led_strip_encoder_encode(rmt_encoder_t *encoder, rmt_channel_handle_t channel,
                                          const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
{
    led_strip_encoder_t *led_encoder = __containerof(encoder, led_strip_encoder_t, base);
    rmt_encoder_handle_t bytes_encoder = led_encoder->bytes_encoder;
    rmt_encoder_handle_t copy_encoder = led_encoder->copy_encoder;
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;

    switch (led_encoder->state) {
        case 0: // Send RGB data
            encoded_symbols = bytes_encoder->encode(bytes_encoder, channel, primary_data, data_size, &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                led_encoder->state = 1; // Move to reset code
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                goto out;
            }
            // Fall through
        case 1: // Send reset code
            encoded_symbols = copy_encoder->encode(copy_encoder, channel, &led_encoder->reset_code,
                                                  sizeof(led_encoder->reset_code), &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                led_encoder->state = 0;
                state |= RMT_ENCODING_COMPLETE;
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                goto out;
            }
    }

out:
    *ret_state = state;
    return ESP_OK;
}

static esp_err_t led_strip_encoder_del(rmt_encoder_t *encoder)
{
    led_strip_encoder_t *led_encoder = __containerof(encoder, led_strip_encoder_t, base);
    rmt_del_encoder(led_encoder->bytes_encoder);
    rmt_del_encoder(led_encoder->copy_encoder);
    free(led_encoder);
    return ESP_OK;
}

static esp_err_t led_strip_encoder_reset(rmt_encoder_t *encoder)
{
    led_strip_encoder_t *led_encoder = __containerof(encoder, led_strip_encoder_t, base);
    rmt_encoder_reset(led_encoder->bytes_encoder);
    rmt_encoder_reset(led_encoder->copy_encoder);
    led_encoder->state = 0;
    return ESP_OK;
}

static esp_err_t led_strip_encoder_new(const led_strip_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder)
{
    led_strip_encoder_t *led_encoder = calloc(1, sizeof(led_strip_encoder_t));
    if (!led_encoder) {
        ESP_LOGE(TAG, "Failed to allocate memory for LED encoder");
        return ESP_ERR_NO_MEM;
    }
    
    led_encoder->base.encode = led_strip_encoder_encode;
    led_encoder->base.del = led_strip_encoder_del;
    led_encoder->base.reset = led_strip_encoder_reset;
    led_encoder->resolution = config->resolution;
    
    // Create byte encoder for WS2812
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = 3, // T0H = 300ns
            .level1 = 0,
            .duration1 = 9, // T0L = 900ns
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = 9, // T1H = 900ns
            .level1 = 0,
            .duration1 = 3, // T1L = 300ns
        },
        .flags.msb_first = 1, // WS2812 expects MSB first
    };
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_config, &led_encoder->bytes_encoder));
    
    // Create copy encoder for reset code
    rmt_copy_encoder_config_t copy_encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&copy_encoder_config, &led_encoder->copy_encoder));
    
    // Reset timing for WS2812 (280us)
    uint32_t reset_ticks = config->resolution / 1000000 * 280 / 2; // 280us reset
    led_encoder->reset_code = (rmt_symbol_word_t) {
        .level0 = 0,
        .duration0 = reset_ticks,
        .level1 = 0,
        .duration1 = reset_ticks,
    };
    
    *ret_encoder = &led_encoder->base;
    return ESP_OK;
}

/* Initialize LED strip hardware */
static esp_err_t led_strip_init(void)
{
    esp_err_t ret = ESP_OK;
    
    // Configure RMT TX channel
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = LED_STRIP_GPIO,
        .mem_block_symbols = 64, // Enough for 4 LEDs
        .resolution_hz = LED_STRIP_RESOLUTION_HZ,
        .trans_queue_depth = 4,
        .flags.invert_out = false,
        .flags.with_dma = false,
    };
    
    ret = rmt_new_tx_channel(&tx_chan_config, &led_state.rmt_chan);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RMT TX channel: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Create LED strip encoder
    led_strip_encoder_config_t encoder_config = {
        .resolution = LED_STRIP_RESOLUTION_HZ,
    };
    
    ret = led_strip_encoder_new(&encoder_config, &led_state.encoder);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create LED strip encoder: %s", esp_err_to_name(ret));
        rmt_del_channel(led_state.rmt_chan);
        return ret;
    }
    
    // Enable RMT channel
    ret = rmt_enable(led_state.rmt_chan);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable RMT channel: %s", esp_err_to_name(ret));
        led_state.encoder->del(led_state.encoder);
        rmt_del_channel(led_state.rmt_chan);
        return ret;
    }
    
    return ESP_OK;
}

/* Deinitialize LED strip hardware */
static esp_err_t led_strip_deinit(void)
{
    if (led_state.rmt_chan) {
        rmt_disable(led_state.rmt_chan);
        rmt_del_channel(led_state.rmt_chan);
        led_state.rmt_chan = NULL;
    }
    
    if (led_state.encoder) {
        led_state.encoder->del(led_state.encoder);
        led_state.encoder = NULL;
    }
    
    return ESP_OK;
}

/* Update physical LED strip with current buffer */
esp_err_t led_strip_refresh(void)
{
    if (!led_state.initialized || !led_state.rmt_chan) {
        return ESP_ERR_INVALID_STATE;
    }
    
    uint8_t pixel_data[LED_COUNT * LED_STRIP_BYTES_PER_PIXEL];
    
    // Convert RGB colors to GRB format expected by WS2812
    for (int i = 0; i < LED_COUNT; i++) {
        pixel_data[i * 3 + 0] = led_state.colors[i].green;
        pixel_data[i * 3 + 1] = led_state.colors[i].red;
        pixel_data[i * 3 + 2] = led_state.colors[i].blue;
    }
    
    // Transmit LED data
    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
        .flags.eot_level = 0,
    };
    
    esp_err_t ret = rmt_transmit(led_state.rmt_chan, led_state.encoder,
                                  pixel_data, sizeof(pixel_data), &tx_config);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to transmit LED data: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

/* Set color for specific LED */
esp_err_t led_set_color(uint8_t led_index, led_color_t color)
{
    if (!led_state.initialized || led_index >= LED_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    led_state.colors[led_index] = color;
    xSemaphoreGive(led_state.mutex);
    
    return led_strip_refresh();
}

/* Set color for all LEDs */
esp_err_t led_set_all(led_color_t color)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    for (int i = 0; i < LED_COUNT; i++) {
        led_state.colors[i] = color;
    }
    xSemaphoreGive(led_state.mutex);
    
    return led_strip_refresh();
}

/* Turn off specific LED */
esp_err_t led_turn_off(uint8_t led_index)
{
    led_color_t off_color = LED_COLOR_OFF;
    return led_set_color(led_index, off_color);
}

/* Turn off all LEDs */
esp_err_t led_turn_off_all(void)
{
    led_color_t off_color = LED_COLOR_OFF;
    return led_set_all(off_color);
}

/* Pattern timer callback */
static void pattern_timer_callback(TimerHandle_t xTimer)
{
    led_color_t color_on, color_off;
    
    switch (led_state.current_pattern) {
        case LED_PATTERN_PAIRING_MODE: // Fast blink blue
            color_on = (led_color_t)LED_COLOR_BLUE;
            color_off = (led_color_t)LED_COLOR_OFF;
            led_set_all((led_state.pattern_state++ % 2) ? color_off : color_on);
            break;
            
        case LED_PATTERN_JOINING: // Slow blink yellow
            color_on = (led_color_t)LED_COLOR_YELLOW;
            color_off = (led_color_t)LED_COLOR_OFF;
            led_set_all((led_state.pattern_state++ % 2) ? color_off : color_on);
            break;
            
        case LED_PATTERN_CONNECTED: // Solid green for 3 seconds
            if (led_state.pattern_count == 0) {
                color_on = (led_color_t)LED_COLOR_GREEN;
                led_set_all(color_on);
                led_state.pattern_count++;
            } else if (led_state.pattern_count++ >= 30) { // 30 * 100ms = 3s
                led_turn_off_all();
                xTimerStop(led_state.pattern_timer, 0);
                led_state.current_pattern = LED_PATTERN_OFF;
            }
            break;
            
        case LED_PATTERN_ERROR: // Double blink red
            color_on = (led_color_t)LED_COLOR_RED;
            color_off = (led_color_t)LED_COLOR_OFF;
            if (led_state.pattern_state < 2) {
                led_set_all(color_on);
            } else if (led_state.pattern_state < 4) {
                led_set_all(color_off);
            } else if (led_state.pattern_state < 6) {
                led_set_all(color_on);
            } else {
                led_set_all(color_off);
            }
            
            if (++led_state.pattern_state >= 10) { // Gap between double blinks
                led_state.pattern_state = 0;
                if (++led_state.pattern_count >= LED_PATTERN_ERROR_REPEAT_COUNT) {
                    xTimerStop(led_state.pattern_timer, 0);
                    led_state.current_pattern = LED_PATTERN_OFF;
                }
            }
            break;
            
        case LED_PATTERN_FACTORY_RESET: // Rapid flash all colors
            switch (led_state.pattern_state++ % 6) {
                case 0: color_on = (led_color_t)LED_COLOR_RED; break;
                case 1: color_on = (led_color_t)LED_COLOR_GREEN; break;
                case 2: color_on = (led_color_t)LED_COLOR_BLUE; break;
                case 3: color_on = (led_color_t)LED_COLOR_YELLOW; break;
                case 4: color_on = (led_color_t)LED_COLOR_CYAN; break;
                case 5: color_on = (led_color_t)LED_COLOR_MAGENTA; break;
            }
            led_set_all(color_on);
            
            if (led_state.pattern_count++ >= LED_PATTERN_FACTORY_RESET_COUNT) {
                xTimerStop(led_state.pattern_timer, 0);
                led_turn_off_all();
                led_state.current_pattern = LED_PATTERN_OFF;
            }
            break;
            
        case LED_PATTERN_STARTUP: // Brief white flash
            if (led_state.pattern_count == 0) {
                color_on = (led_color_t)LED_COLOR_WHITE;
                led_set_all(color_on);
                led_state.pattern_count++;
            } else if (led_state.pattern_count++ >= 5) { // 500ms
                led_turn_off_all();
                xTimerStop(led_state.pattern_timer, 0);
                led_state.current_pattern = LED_PATTERN_OFF;
            }
            break;
            
        default:
            xTimerStop(led_state.pattern_timer, 0);
            led_turn_off_all();
            break;
    }
}

/* Initialize LED indicator system */
esp_err_t led_indicator_init(void)
{
    if (led_state.initialized) {
        ESP_LOGW(TAG, "LED indicator already initialized");
        return ESP_OK;
    }
    
    esp_err_t ret = ESP_OK;
    
    // Create mutex
    led_state.mutex = xSemaphoreCreateMutex();
    if (!led_state.mutex) {
        ESP_LOGE(TAG, "Failed to create mutex");
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize LED strip hardware
    ret = led_strip_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize LED strip: %s", esp_err_to_name(ret));
        vSemaphoreDelete(led_state.mutex);
        return ret;
    }
    
    // Create pattern timer
    led_state.pattern_timer = xTimerCreate("led_pattern",
                                           pdMS_TO_TICKS(100),
                                           pdTRUE,
                                           NULL,
                                           pattern_timer_callback);
    
    if (!led_state.pattern_timer) {
        ESP_LOGE(TAG, "Failed to create pattern timer");
        led_strip_deinit();
        vSemaphoreDelete(led_state.mutex);
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize all LEDs to off
    memset(led_state.colors, 0, sizeof(led_state.colors));
    led_strip_refresh();
    
    led_state.initialized = true;
    ESP_LOGI(TAG, "LED indicator initialized with WS2812 strip on GPIO %d", LED_STRIP_GPIO);
    
    return ESP_OK;
}

/* Deinitialize LED indicator system */
void led_indicator_deinit(void)
{
    if (!led_state.initialized) {
        return;
    }
    
    // Stop any active patterns
    if (led_state.pattern_timer) {
        xTimerStop(led_state.pattern_timer, 0);
        xTimerDelete(led_state.pattern_timer, 0);
        led_state.pattern_timer = NULL;
    }
    
    // Turn off all LEDs
    led_turn_off_all();
    
    // Deinitialize hardware
    led_strip_deinit();
    
    // Delete mutex
    if (led_state.mutex) {
        vSemaphoreDelete(led_state.mutex);
        led_state.mutex = NULL;
    }
    
    led_state.initialized = false;
    ESP_LOGI(TAG, "LED indicator deinitialized");
}

/* Start LED pattern for pairing mode */
esp_err_t led_indicate_pairing_mode(void)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "LED: Pairing mode indication");
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    led_state.current_pattern = LED_PATTERN_PAIRING_MODE;
    led_state.pattern_state = 0;
    led_state.pattern_count = 0;
    xSemaphoreGive(led_state.mutex);
    
    xTimerChangePeriod(led_state.pattern_timer,
                        pdMS_TO_TICKS(LED_PATTERN_FAST_BLINK_ON_MS), 0);
    xTimerStart(led_state.pattern_timer, 0);
    
    return ESP_OK;
}

/* Start LED pattern for joining network */
esp_err_t led_indicate_joining(void)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "LED: Joining network indication");
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    led_state.current_pattern = LED_PATTERN_JOINING;
    led_state.pattern_state = 0;
    led_state.pattern_count = 0;
    xSemaphoreGive(led_state.mutex);
    
    xTimerChangePeriod(led_state.pattern_timer,
                        pdMS_TO_TICKS(LED_PATTERN_SLOW_BLINK_ON_MS), 0);
    xTimerStart(led_state.pattern_timer, 0);
    
    return ESP_OK;
}

/* Start LED pattern for successful connection */
esp_err_t led_indicate_connected(void)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "LED: Connected indication");
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    led_state.current_pattern = LED_PATTERN_CONNECTED;
    led_state.pattern_state = 0;
    led_state.pattern_count = 0;
    xSemaphoreGive(led_state.mutex);
    
    xTimerChangePeriod(led_state.pattern_timer,
                        pdMS_TO_TICKS(100), 0);
    xTimerStart(led_state.pattern_timer, 0);
    
    return ESP_OK;
}

/* Start LED pattern for error */
esp_err_t led_indicate_error(void)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "LED: Error indication");
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    led_state.current_pattern = LED_PATTERN_ERROR;
    led_state.pattern_state = 0;
    led_state.pattern_count = 0;
    xSemaphoreGive(led_state.mutex);
    
    xTimerChangePeriod(led_state.pattern_timer,
                        pdMS_TO_TICKS(LED_PATTERN_DOUBLE_BLINK_ON_MS), 0);
    xTimerStart(led_state.pattern_timer, 0);
    
    return ESP_OK;
}

/* Start LED pattern for factory reset */
esp_err_t led_indicate_factory_reset(void)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "LED: Factory reset indication");
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    led_state.current_pattern = LED_PATTERN_FACTORY_RESET;
    led_state.pattern_state = 0;
    led_state.pattern_count = 0;
    xSemaphoreGive(led_state.mutex);
    
    xTimerChangePeriod(led_state.pattern_timer,
                        pdMS_TO_TICKS(LED_PATTERN_RAPID_FLASH_ON_MS), 0);
    xTimerStart(led_state.pattern_timer, 0);
    
    return ESP_OK;
}

/* Quick flash for button press acknowledgment */
esp_err_t led_indicate_button_press(uint8_t button_num)
{
    if (!led_state.initialized || button_num < 1 || button_num > 4) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGD(TAG, "LED: Button %d press indication", button_num);
    
    // Flash the specific LED for this button
    led_color_t white = LED_COLOR_WHITE;
    led_set_color(button_num - 1, white);
    vTaskDelay(pdMS_TO_TICKS(LED_PATTERN_QUICK_FLASH_MS));
    led_turn_off(button_num - 1);
    
    return ESP_OK;
}

/* Start pulsing pattern for button hold */
esp_err_t led_indicate_button_hold(uint8_t button_num)
{
    if (!led_state.initialized || button_num < 1 || button_num > 4) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGD(TAG, "LED: Button %d hold indication", button_num);
    
    // Pulse the specific LED for this button
    led_color_t blue = LED_COLOR_BLUE;
    led_set_color(button_num - 1, blue);
    
    return ESP_OK;
}

/* Special pattern when pairing mode threshold is reached */
esp_err_t led_indicate_pairing_threshold(void)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "LED: Pairing threshold reached");
    
    // Flash all LEDs cyan to indicate pairing mode activated
    led_color_t cyan = LED_COLOR_CYAN;
    led_set_all(cyan);
    vTaskDelay(pdMS_TO_TICKS(500));
    led_turn_off_all();
    vTaskDelay(pdMS_TO_TICKS(200));
    led_set_all(cyan);
    vTaskDelay(pdMS_TO_TICKS(500));
    led_turn_off_all();
    
    return ESP_OK;
}

/* Show startup pattern */
esp_err_t led_indicate_startup(void)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "LED: Startup indication");
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    led_state.current_pattern = LED_PATTERN_STARTUP;
    led_state.pattern_state = 0;
    led_state.pattern_count = 0;
    xSemaphoreGive(led_state.mutex);
    
    xTimerChangePeriod(led_state.pattern_timer,
                        pdMS_TO_TICKS(100), 0);
    xTimerStart(led_state.pattern_timer, 0);
    
    return ESP_OK;
}

/* Stop current LED pattern */
esp_err_t led_indicator_stop(void)
{
    if (!led_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    xTimerStop(led_state.pattern_timer, 0);
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    led_state.current_pattern = LED_PATTERN_OFF;
    led_state.pattern_state = 0;
    led_state.pattern_count = 0;
    xSemaphoreGive(led_state.mutex);
    
    return led_turn_off_all();
}

/* Set a custom LED pattern */
esp_err_t led_indicator_set_pattern(led_pattern_t pattern)
{
    switch (pattern) {
        case LED_PATTERN_PAIRING_MODE:
            return led_indicate_pairing_mode();
        case LED_PATTERN_JOINING:
            return led_indicate_joining();
        case LED_PATTERN_CONNECTED:
            return led_indicate_connected();
        case LED_PATTERN_ERROR:
            return led_indicate_error();
        case LED_PATTERN_FACTORY_RESET:
            return led_indicate_factory_reset();
        case LED_PATTERN_STARTUP:
            return led_indicate_startup();
        case LED_PATTERN_OFF:
        default:
            return led_indicator_stop();
    }
}

/* Get current active pattern */
led_pattern_t led_indicator_get_pattern(void)
{
    led_pattern_t pattern;
    
    xSemaphoreTake(led_state.mutex, portMAX_DELAY);
    pattern = led_state.current_pattern;
    xSemaphoreGive(led_state.mutex);
    
    return pattern;
}

/* Check if LED indicator is currently active */
bool led_indicator_is_active(void)
{
    return led_state.initialized && (led_state.current_pattern != LED_PATTERN_OFF);
}