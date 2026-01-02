/*
 * BTN-1 Button Handler Implementation
 * GPIO configuration and event handling for 4-button macro pad
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "button_handler.h"
#include "power_management.h"

static const char *TAG = "BUTTON_HANDLER";

/* Button GPIO mapping */
static const gpio_num_t button_gpio_map[BUTTON_COUNT] = {
    BUTTON_1_GPIO,
    BUTTON_2_GPIO,
    BUTTON_3_GPIO,
    BUTTON_4_GPIO
};

/* Button state tracking */
static button_state_t button_states[BUTTON_COUNT];

/* FreeRTOS queue for button events */
static QueueHandle_t button_event_queue = NULL;

/* Task handle */
static TaskHandle_t button_task_handle = NULL;

/* Button event callback */
static button_event_callback_t event_callback = NULL;

/* Get current time in milliseconds */
static uint32_t IRAM_ATTR get_time_ms(void)
{
    return esp_timer_get_time() / 1000;
}

/* GPIO interrupt service routine */
static void IRAM_ATTR button_isr_handler(void *arg)
{
    uint8_t button_num = (uint8_t)(uintptr_t)arg;
    
    button_event_t event = {
        .button_num = button_num,
        .timestamp = get_time_ms()
    };
    
    /* Read button state (active low with pull-up) */
    int level = gpio_get_level(button_gpio_map[button_num - 1]);
    
    if (level == 0) {
        /* Button pressed */
        event.event = BUTTON_EVENT_PRESS;
    } else {
        /* Button released */
        event.event = BUTTON_EVENT_RELEASE;
    }
    
    /* Send event to queue from ISR */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(button_event_queue, &event, &xHigherPriorityTaskWoken);
    
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

/* Process button events and handle debouncing */
static void process_button_event(button_event_t *raw_event)
{
    if (raw_event->button_num < 1 || raw_event->button_num > BUTTON_COUNT) {
        return;
    }
    
    uint8_t idx = raw_event->button_num - 1;
    button_state_t *state = &button_states[idx];
    uint32_t current_time = raw_event->timestamp;
    
    if (raw_event->event == BUTTON_EVENT_PRESS) {
        /* Debounce check */
        if (state->is_pressed || 
            (current_time - state->last_press_time) < BUTTON_DEBOUNCE_TIME_MS) {
            return;
        }
        
        state->is_pressed = 1;
        state->press_start_time = current_time;
        state->long_press_handled = 0;
        
        /* Check for double click */
        if ((current_time - state->last_press_time) < BUTTON_DOUBLE_CLICK_TIME_MS) {
            state->press_count++;
        } else {
            state->press_count = 1;
        }
        
        state->last_press_time = current_time;
        
    } else if (raw_event->event == BUTTON_EVENT_RELEASE) {
        /* Debounce check */
        if (!state->is_pressed) {
            return;
        }
        
        state->is_pressed = 0;
        uint32_t press_duration = current_time - state->press_start_time;
        
        /* Generate appropriate event */
        button_event_t processed_event = {
            .button_num = raw_event->button_num,
            .timestamp = current_time
        };
        
        if (state->long_press_handled) {
            /* Long press was already handled, no additional event */
            return;
        }
        
        if (state->press_count >= 2) {
            /* Double click detected */
            processed_event.event = BUTTON_EVENT_DOUBLE_PRESS;
            state->press_count = 0;  /* Reset count */
        } else if (press_duration < BUTTON_LONG_PRESS_TIME_MS) {
            /* Short press - but wait a bit to see if double click follows */
            /* This will be handled by the timeout in the main task */
            return;
        }
        
        /* Send processed event to callback */
        if (event_callback) {
            /* Extend wake lock for command processing */
            power_acquire_wake_lock(WAKE_LOCK_BUTTON, 3000);
            event_callback(&processed_event);
        }
    }
}

/* Check for long press and delayed short press */
static void check_button_timeouts(void)
{
    uint32_t current_time = get_time_ms();
    
    for (int i = 0; i < BUTTON_COUNT; i++) {
        button_state_t *state = &button_states[i];
        
        /* Check for long press */
        if (state->is_pressed && !state->long_press_handled) {
            uint32_t press_duration = current_time - state->press_start_time;
            
            if (press_duration >= BUTTON_LONG_PRESS_TIME_MS) {
                /* Generate long press event */
                button_event_t event = {
                    .button_num = i + 1,
                    .event = BUTTON_EVENT_LONG_PRESS,
                    .timestamp = current_time
                };
                
                state->long_press_handled = 1;
                
                if (event_callback) {
                    /* Extend wake lock for command processing */
                    power_acquire_wake_lock(WAKE_LOCK_BUTTON, 3000);
                    event_callback(&event);
                }
            }
        }
        
        /* Check for delayed short press (waiting for double click timeout) */
        if (!state->is_pressed && state->press_count == 1) {
            uint32_t time_since_press = current_time - state->last_press_time;
            
            if (time_since_press >= BUTTON_DOUBLE_CLICK_TIME_MS) {
                /* Generate short press event */
                button_event_t event = {
                    .button_num = i + 1,
                    .event = BUTTON_EVENT_SHORT_PRESS,
                    .timestamp = current_time
                };
                
                state->press_count = 0;  /* Reset count */
                
                if (event_callback) {
                    /* Extend wake lock for command processing */
                    power_acquire_wake_lock(WAKE_LOCK_BUTTON, 3000);
                    event_callback(&event);
                }
            }
        }
    }
}

/* Button handler task */
static void button_handler_task(void *arg)
{
    button_event_t event;
    
    ESP_LOGI(TAG, "Button handler task started");
    
    while (1) {
        /* Check if we woke from sleep due to button press */
        esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
        if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
            ESP_LOGI(TAG, "Woke from GPIO button press");
            power_on_button_wake();
            
            /* Acquire wake lock for button processing */
            power_acquire_wake_lock(WAKE_LOCK_BUTTON, 2000);
        }
        
        /* Wait for button events with timeout for checking long press */
        if (xQueueReceive(button_event_queue, &event, pdMS_TO_TICKS(50)) == pdTRUE) {
            /* Acquire wake lock for button processing */
            power_acquire_wake_lock(WAKE_LOCK_BUTTON, 2000);
            
            /* Process received event */
            process_button_event(&event);
        }
        
        /* Check for timeouts (long press and delayed short press) */
        check_button_timeouts();
        
        /* Release wake lock if no ongoing button activity */
        bool any_button_pressed = false;
        for (int i = 0; i < BUTTON_COUNT; i++) {
            if (button_states[i].is_pressed) {
                any_button_pressed = true;
                break;
            }
        }
        if (!any_button_pressed) {
            power_release_wake_lock(WAKE_LOCK_BUTTON);
        }
    }
}

/* Initialize button handler */
esp_err_t button_handler_init(void)
{
    ESP_LOGI(TAG, "Initializing button handler");
    
    /* Create event queue */
    button_event_queue = xQueueCreate(10, sizeof(button_event_t));
    if (button_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create button event queue");
        return ESP_ERR_NO_MEM;
    }
    
    /* Initialize button states */
    memset(button_states, 0, sizeof(button_states));
    
    /* Configure GPIO pins */
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,  /* Interrupt on both edges */
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 0,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE  /* Enable internal pull-up */
    };
    
    /* Configure each button GPIO */
    for (int i = 0; i < BUTTON_COUNT; i++) {
        io_conf.pin_bit_mask = (1ULL << button_gpio_map[i]);
        esp_err_t ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure GPIO %d: %s", 
                     button_gpio_map[i], esp_err_to_name(ret));
            return ret;
        }
        
        /* Install ISR service if not already installed */
        static bool isr_service_installed = false;
        if (!isr_service_installed) {
            ret = gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM);
            if (ret == ESP_OK) {
                isr_service_installed = true;
            } else if (ret != ESP_ERR_INVALID_STATE) {
                ESP_LOGE(TAG, "Failed to install ISR service: %s", esp_err_to_name(ret));
                return ret;
            }
        }
        
        /* Add ISR handler for button */
        ret = gpio_isr_handler_add(button_gpio_map[i], button_isr_handler, 
                                   (void *)(uintptr_t)(i + 1));
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to add ISR handler for GPIO %d: %s",
                     button_gpio_map[i], esp_err_to_name(ret));
            return ret;
        }
        
        ESP_LOGI(TAG, "Configured button %d on GPIO %d", i + 1, button_gpio_map[i]);
    }
    
    return ESP_OK;
}

/* Start button handler task */
esp_err_t button_handler_start(void)
{
    if (button_task_handle != NULL) {
        ESP_LOGW(TAG, "Button handler task already started");
        return ESP_OK;
    }
    
    /* Create button handler task */
    BaseType_t ret = xTaskCreate(button_handler_task, "button_handler",
                                 2048, NULL, 10, &button_task_handle);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create button handler task");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "Button handler started");
    return ESP_OK;
}

/* Deinitialize button handler */
void button_handler_deinit(void)
{
    /* Delete task */
    if (button_task_handle != NULL) {
        vTaskDelete(button_task_handle);
        button_task_handle = NULL;
    }
    
    /* Remove ISR handlers */
    for (int i = 0; i < BUTTON_COUNT; i++) {
        gpio_isr_handler_remove(button_gpio_map[i]);
    }
    
    /* Delete queue */
    if (button_event_queue != NULL) {
        vQueueDelete(button_event_queue);
        button_event_queue = NULL;
    }
    
    ESP_LOGI(TAG, "Button handler deinitialized");
}

/* Register callback for button events */
esp_err_t button_register_callback(button_event_callback_t callback)
{
    if (callback == NULL) {
        ESP_LOGW(TAG, "Attempting to register NULL callback");
        return ESP_ERR_INVALID_ARG;
    }
    
    event_callback = callback;
    ESP_LOGI(TAG, "Button event callback registered");
    return ESP_OK;
}

/* Get button GPIO from button number */
gpio_num_t button_get_gpio(uint8_t button_num)
{
    if (button_num < 1 || button_num > BUTTON_COUNT) {
        return GPIO_NUM_NC;
    }
    
    return button_gpio_map[button_num - 1];
}