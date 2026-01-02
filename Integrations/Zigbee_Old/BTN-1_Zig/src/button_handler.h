/*
 * BTN-1 Button Handler
 * GPIO configuration and event handling for 4-button macro pad
 */

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"

/* GPIO Pin Definitions for Buttons */
#define BUTTON_1_GPIO   GPIO_NUM_4
#define BUTTON_2_GPIO   GPIO_NUM_5
#define BUTTON_3_GPIO   GPIO_NUM_6
#define BUTTON_4_GPIO   GPIO_NUM_7

/* Button count */
#define BUTTON_COUNT    4

/* Timing constants (in milliseconds) */
#define BUTTON_DEBOUNCE_TIME_MS     50
#define BUTTON_LONG_PRESS_TIME_MS   500
#define BUTTON_DOUBLE_CLICK_TIME_MS 300

/* Button event types */
typedef enum {
    BUTTON_EVENT_SHORT_PRESS,
    BUTTON_EVENT_LONG_PRESS,
    BUTTON_EVENT_DOUBLE_PRESS,
    BUTTON_EVENT_PRESS,         /* Initial press */
    BUTTON_EVENT_RELEASE        /* Button release */
} button_event_type_t;

/* Button event structure */
typedef struct {
    uint8_t button_num;         /* Button number (1-4) */
    button_event_type_t event;  /* Type of event */
    uint32_t timestamp;         /* Event timestamp */
} button_event_t;

/* Button state tracking structure */
typedef struct {
    uint32_t last_press_time;
    uint32_t press_start_time;
    uint8_t press_count;
    uint8_t is_pressed;
    uint8_t long_press_handled;
} button_state_t;

/* Function prototypes */
esp_err_t button_handler_init(void);
esp_err_t button_handler_start(void);
void button_handler_deinit(void);

/* Callback type for button events */
typedef void (*button_event_callback_t)(button_event_t *event);

/* Register callback for button events */
esp_err_t button_register_callback(button_event_callback_t callback);

/* Get button GPIO from button number */
gpio_num_t button_get_gpio(uint8_t button_num);

#endif /* BUTTON_HANDLER_H */