/*
 * BTN-1 LED Indicator Module
 * Visual feedback system for network and button operations
 * Uses WS2812 addressable RGB LED strip with 4 LEDs
 */

#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"

/* LED GPIO Configuration */
#define LED_STRIP_GPIO      GPIO_NUM_21  /* WS2812 RGB LED strip on GPIO21 */
#define LED_COUNT           4             /* 4 LEDs in the strip */
#define LED_RMT_CHANNEL     0             /* RMT channel for LED control */

/* LED indices for each button */
#define LED_BUTTON_1        0
#define LED_BUTTON_2        1
#define LED_BUTTON_3        2
#define LED_BUTTON_4        3

/* LED Pattern Timing Configuration (in milliseconds) */
#define LED_PATTERN_FAST_BLINK_ON_MS       100
#define LED_PATTERN_FAST_BLINK_OFF_MS      100
#define LED_PATTERN_SLOW_BLINK_ON_MS       500
#define LED_PATTERN_SLOW_BLINK_OFF_MS      500
#define LED_PATTERN_DOUBLE_BLINK_ON_MS     150
#define LED_PATTERN_DOUBLE_BLINK_OFF_MS    150
#define LED_PATTERN_DOUBLE_BLINK_GAP_MS    500
#define LED_PATTERN_RAPID_FLASH_ON_MS      50
#define LED_PATTERN_RAPID_FLASH_OFF_MS     50
#define LED_PATTERN_CONNECTED_DURATION_MS  3000
#define LED_PATTERN_QUICK_FLASH_MS         50
#define LED_PATTERN_LONG_PRESS_PULSE_MS    200

/* Pattern repeat counts */
#define LED_PATTERN_ERROR_REPEAT_COUNT      3
#define LED_PATTERN_FACTORY_RESET_COUNT     20

/* LED Colors (RGB values 0-255) */
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} led_color_t;

/* Predefined colors */
#define LED_COLOR_RED       {255, 0, 0}
#define LED_COLOR_GREEN     {0, 255, 0}
#define LED_COLOR_BLUE      {0, 0, 255}
#define LED_COLOR_YELLOW    {255, 255, 0}
#define LED_COLOR_CYAN      {0, 255, 255}
#define LED_COLOR_MAGENTA   {255, 0, 255}
#define LED_COLOR_WHITE     {255, 255, 255}
#define LED_COLOR_OFF       {0, 0, 0}

/* LED Pattern Types */
typedef enum {
    LED_PATTERN_OFF = 0,
    LED_PATTERN_PAIRING_MODE,      /* Fast blink - searching for network */
    LED_PATTERN_JOINING,            /* Slow blink - joining network */
    LED_PATTERN_CONNECTED,          /* Solid on for 3 seconds then off */
    LED_PATTERN_ERROR,              /* Double blink pattern */
    LED_PATTERN_FACTORY_RESET,      /* Rapid flash pattern */
    LED_PATTERN_BUTTON_PRESS,       /* Quick flash acknowledgment */
    LED_PATTERN_BUTTON_HOLD,        /* Pulsing during long press */
    LED_PATTERN_PAIRING_THRESHOLD,  /* Pattern when pairing threshold reached */
    LED_PATTERN_STARTUP,            /* Brief flash on startup */
} led_pattern_t;

/* Function prototypes */

/**
 * @brief Initialize the LED indicator system
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t led_indicator_init(void);

/**
 * @brief Deinitialize the LED indicator system
 */
void led_indicator_deinit(void);

/**
 * @brief Start LED pattern for pairing mode (fast blink)
 * 
 * @return ESP_OK on success
 */
esp_err_t led_indicate_pairing_mode(void);

/**
 * @brief Start LED pattern for joining network (slow blink)
 * 
 * @return ESP_OK on success
 */
esp_err_t led_indicate_joining(void);

/**
 * @brief Start LED pattern for successful connection (solid 3s then off)
 * 
 * @return ESP_OK on success
 */
esp_err_t led_indicate_connected(void);

/**
 * @brief Start LED pattern for error (double blink)
 * 
 * @return ESP_OK on success
 */
esp_err_t led_indicate_error(void);

/**
 * @brief Start LED pattern for factory reset (rapid flash)
 * 
 * @return ESP_OK on success
 */
esp_err_t led_indicate_factory_reset(void);

/**
 * @brief Quick flash for button press acknowledgment
 *
 * @param button_num Button number (1-4) that was pressed
 * @return ESP_OK on success
 */
esp_err_t led_indicate_button_press(uint8_t button_num);

/**
 * @brief Start pulsing pattern for button hold
 *
 * @param button_num Button number (1-4) being held
 * @return ESP_OK on success
 */
esp_err_t led_indicate_button_hold(uint8_t button_num);

/**
 * @brief Set color for a specific LED
 *
 * @param led_index LED index (0-3)
 * @param color Color to set
 * @return ESP_OK on success
 */
esp_err_t led_set_color(uint8_t led_index, led_color_t color);

/**
 * @brief Set color for all LEDs
 *
 * @param color Color to set
 * @return ESP_OK on success
 */
esp_err_t led_set_all(led_color_t color);

/**
 * @brief Turn off a specific LED
 *
 * @param led_index LED index (0-3)
 * @return ESP_OK on success
 */
esp_err_t led_turn_off(uint8_t led_index);

/**
 * @brief Turn off all LEDs
 *
 * @return ESP_OK on success
 */
esp_err_t led_turn_off_all(void);

/**
 * @brief Update LED strip with current buffer
 *
 * @return ESP_OK on success
 */
esp_err_t led_strip_refresh(void);

/**
 * @brief Special pattern when pairing mode threshold is reached
 * 
 * @return ESP_OK on success
 */
esp_err_t led_indicate_pairing_threshold(void);

/**
 * @brief Show startup pattern
 * 
 * @return ESP_OK on success
 */
esp_err_t led_indicate_startup(void);

/**
 * @brief Stop current LED pattern and turn off LED
 * 
 * @return ESP_OK on success
 */
esp_err_t led_indicator_stop(void);

/**
 * @brief Set a custom LED pattern
 * 
 * @param pattern Pattern type to set
 * @return ESP_OK on success
 */
esp_err_t led_indicator_set_pattern(led_pattern_t pattern);

/**
 * @brief Get current active pattern
 * 
 * @return Current LED pattern
 */
led_pattern_t led_indicator_get_pattern(void);

/**
 * @brief Check if LED indicator is currently active
 * 
 * @return true if pattern is running, false otherwise
 */
bool led_indicator_is_active(void);

#endif /* LED_INDICATOR_H */