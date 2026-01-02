// Simple LED indicator using GPIO (no WS2812) for ESP-IDF v5.2.1 compatibility
#include "simple_led.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"

static const char *TAG = "SIMPLE_LED";

// Use GPIO 8 for onboard LED (common on ESP32-C6 dev boards)
// If no onboard LED, we could use GPIO 21 with a simple LED (not WS2812)
#define LED_GPIO 8

static TimerHandle_t led_timer = NULL;
static led_pattern_t current_pattern = LED_PATTERN_OFF;
static uint8_t pattern_counter = 0;

static void led_timer_callback(TimerHandle_t xTimer);

void simple_led_init(void) {
    ESP_LOGI(TAG, "Initializing simple LED on GPIO %d", LED_GPIO);
    
    // Configure GPIO for LED
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // Turn LED off initially
    gpio_set_level(LED_GPIO, 0);
    
    // Create timer for patterns
    led_timer = xTimerCreate("led_timer", pdMS_TO_TICKS(100), pdTRUE, NULL, led_timer_callback);
    if (led_timer == NULL) {
        ESP_LOGE(TAG, "Failed to create LED timer");
    }
    
    ESP_LOGI(TAG, "Simple LED initialized");
}

static void led_timer_callback(TimerHandle_t xTimer) {
    static bool led_state = false;
    
    switch (current_pattern) {
        case LED_PATTERN_PAIRING:
            // Fast blink - toggle every 100ms
            led_state = !led_state;
            gpio_set_level(LED_GPIO, led_state ? 1 : 0);
            break;
            
        case LED_PATTERN_JOINING:
            // Slow blink - toggle every 500ms (5 timer ticks)
            if (pattern_counter % 5 == 0) {
                led_state = !led_state;
                gpio_set_level(LED_GPIO, led_state ? 1 : 0);
            }
            pattern_counter++;
            break;
            
        case LED_PATTERN_ERROR:
            // Double blink pattern
            if (pattern_counter < 2) {
                gpio_set_level(LED_GPIO, 1);
            } else if (pattern_counter < 4) {
                gpio_set_level(LED_GPIO, 0);
            } else if (pattern_counter < 6) {
                gpio_set_level(LED_GPIO, 1);
            } else {
                gpio_set_level(LED_GPIO, 0);
                if (pattern_counter >= 20) {  // 2 second cycle
                    pattern_counter = 0;
                }
            }
            pattern_counter++;
            break;
            
        case LED_PATTERN_FACTORY_RESET:
            // Rapid flash - toggle every timer tick (100ms)
            led_state = !led_state;
            gpio_set_level(LED_GPIO, led_state ? 1 : 0);
            break;
            
        default:
            gpio_set_level(LED_GPIO, 0);
            break;
    }
}

void simple_led_set_pattern(led_pattern_t pattern) {
    current_pattern = pattern;
    pattern_counter = 0;
    
    if (pattern == LED_PATTERN_OFF) {
        if (led_timer) {
            xTimerStop(led_timer, 0);
        }
        gpio_set_level(LED_GPIO, 0);
    } else if (pattern == LED_PATTERN_CONNECTED) {
        // Solid on for 3 seconds
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(3000));
        gpio_set_level(LED_GPIO, 0);
        current_pattern = LED_PATTERN_OFF;
    } else {
        if (led_timer) {
            xTimerStart(led_timer, 0);
        }
    }
}

void simple_led_indicate_button_press(void) {
    // Quick flash (50ms on, then return to previous state)
    gpio_set_level(LED_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // Return to previous pattern state
    if (current_pattern == LED_PATTERN_OFF) {
        gpio_set_level(LED_GPIO, 0);
    }
}