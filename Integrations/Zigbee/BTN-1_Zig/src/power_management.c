/*
 * BTN-1 Power Management Implementation
 * Manages ESP32-C6 power states for optimal battery life
 */

#include <string.h>
#include "power_management.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/adc.h"
#include "hal/adc_types.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_zigbee_core.h"

static const char *TAG = "POWER_MGMT";

/* Power management state */
typedef struct {
    power_config_t config;
    power_state_t state;
    power_stats_t stats;
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t wake_locks[WAKE_LOCK_COUNT];
    TimerHandle_t wake_lock_timers[WAKE_LOCK_COUNT];
    TimerHandle_t activity_timer;
    esp_timer_handle_t battery_timer;
    bool initialized;
    bool debug_logging;
    bool zigbee_can_sleep;
    esp_pm_lock_handle_t pm_lock_cpu_freq_max;
    uint32_t adc_cali_handle; /* ADC calibration handle */
} power_mgmt_state_t;

static power_mgmt_state_t g_pm_state = {0};

/* Default configuration */
static const power_config_t default_config = {
    .activity_timeout_ms = PM_ACTIVITY_TIMEOUT_MS,
    .min_sleep_duration_ms = 100,
    .adaptive_sleep_enabled = true,
    .battery_monitoring_enabled = true,
};

/* Forward declarations */
static void activity_timer_callback(TimerHandle_t xTimer);
static void wake_lock_timer_callback(TimerHandle_t xTimer);
static void battery_timer_callback(void* arg);
static esp_err_t configure_gpio_wakeup(void);
static void enter_light_sleep(void);
static void update_power_state(power_state_t new_state);

/* Get current time in milliseconds */
static uint32_t get_time_ms(void)
{
    return esp_timer_get_time() / 1000;
}

/* Initialize power management system */
esp_err_t power_management_init(const power_config_t *config)
{
    if (g_pm_state.initialized) {
        ESP_LOGW(TAG, "Power management already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing power management");

    /* Copy configuration */
    if (config) {
        memcpy(&g_pm_state.config, config, sizeof(power_config_t));
    } else {
        memcpy(&g_pm_state.config, &default_config, sizeof(power_config_t));
    }

    /* Create mutex */
    g_pm_state.mutex = xSemaphoreCreateMutex();
    if (!g_pm_state.mutex) {
        ESP_LOGE(TAG, "Failed to create mutex");
        return ESP_ERR_NO_MEM;
    }

    /* Create wake lock semaphores */
    for (int i = 0; i < WAKE_LOCK_COUNT; i++) {
        g_pm_state.wake_locks[i] = xSemaphoreCreateBinary();
        if (!g_pm_state.wake_locks[i]) {
            ESP_LOGE(TAG, "Failed to create wake lock semaphore");
            return ESP_ERR_NO_MEM;
        }
    }

    /* Configure ESP32 power management */
    esp_pm_config_esp32c6_t pm_config = {
        .max_freq_mhz = PM_MAX_CPU_FREQ,
        .min_freq_mhz = PM_MIN_CPU_FREQ,
        .light_sleep_enable = PM_LIGHT_SLEEP_ENABLE
    };

    esp_err_t ret = esp_pm_configure(&pm_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure power management: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Create CPU frequency lock for active operations */
    ret = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "btn1_cpu_max", &g_pm_state.pm_lock_cpu_freq_max);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create PM lock: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Configure GPIO wake-up sources */
    ret = configure_gpio_wakeup();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO wake-up: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Create activity timer */
    g_pm_state.activity_timer = xTimerCreate("activity_timer",
                                           pdMS_TO_TICKS(g_pm_state.config.activity_timeout_ms),
                                           pdFALSE,
                                           NULL,
                                           activity_timer_callback);
    if (!g_pm_state.activity_timer) {
        ESP_LOGE(TAG, "Failed to create activity timer");
        return ESP_ERR_NO_MEM;
    }

    /* Initialize battery monitoring if enabled */
    if (g_pm_state.config.battery_monitoring_enabled) {
        /* Configure ADC for battery monitoring */
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(PM_BATTERY_ADC_CHANNEL, ADC_ATTEN_DB_12);
        
        /* For ESP32-C6, ADC calibration is handled internally */
        g_pm_state.adc_cali_handle = 0;

        /* Create battery monitoring timer */
        const esp_timer_create_args_t battery_timer_args = {
            .callback = battery_timer_callback,
            .name = "battery_timer"
        };
        esp_timer_create(&battery_timer_args, &g_pm_state.battery_timer);
        esp_timer_start_periodic(g_pm_state.battery_timer, PM_BATTERY_CHECK_INTERVAL_MS * 1000);
    }

    /* Initialize statistics */
    memset(&g_pm_state.stats, 0, sizeof(power_stats_t));
    g_pm_state.stats.last_activity_time_ms = get_time_ms();

    g_pm_state.state = POWER_STATE_ACTIVE;
    g_pm_state.initialized = true;
    g_pm_state.zigbee_can_sleep = false;

    ESP_LOGI(TAG, "Power management initialized (CPU: %d-%d MHz, Light sleep: %s)",
             PM_MIN_CPU_FREQ, PM_MAX_CPU_FREQ, PM_LIGHT_SLEEP_ENABLE ? "enabled" : "disabled");

    /* Start activity timer */
    xTimerStart(g_pm_state.activity_timer, 0);

    return ESP_OK;
}

/* Configure GPIO wake-up sources */
static esp_err_t configure_gpio_wakeup(void)
{
    /* Configure wake-up on all button GPIOs (from button_handler.h) */
    /* Buttons are on GPIO 0, 1, 2, 3 */
    gpio_num_t button_gpios[] = {GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3};
    
    uint64_t gpio_mask = 0;
    for (int i = 0; i < 4; i++) {
        gpio_mask |= (1ULL << button_gpios[i]);
    }

    /* Enable wake on low level (buttons are pulled up, active low) */
    /* For ESP32-C6, use ext1 wakeup with low level trigger */
    esp_err_t ret = esp_sleep_enable_ext1_wakeup(gpio_mask, ESP_EXT1_WAKEUP_ALL_LOW);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable GPIO wake-up: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Configured wake-up on GPIO mask 0x%llx", gpio_mask);
    return ESP_OK;
}

/* Deinitialize power management */
void power_management_deinit(void)
{
    if (!g_pm_state.initialized) {
        return;
    }

    ESP_LOGI(TAG, "Deinitializing power management");

    /* Stop timers */
    if (g_pm_state.activity_timer) {
        xTimerDelete(g_pm_state.activity_timer, 0);
    }

    if (g_pm_state.battery_timer) {
        esp_timer_stop(g_pm_state.battery_timer);
        esp_timer_delete(g_pm_state.battery_timer);
    }

    /* Delete wake lock timers */
    for (int i = 0; i < WAKE_LOCK_COUNT; i++) {
        if (g_pm_state.wake_lock_timers[i]) {
            xTimerDelete(g_pm_state.wake_lock_timers[i], 0);
        }
        if (g_pm_state.wake_locks[i]) {
            vSemaphoreDelete(g_pm_state.wake_locks[i]);
        }
    }

    /* Delete PM lock */
    if (g_pm_state.pm_lock_cpu_freq_max) {
        esp_pm_lock_delete(g_pm_state.pm_lock_cpu_freq_max);
    }

    /* Delete mutex */
    if (g_pm_state.mutex) {
        vSemaphoreDelete(g_pm_state.mutex);
    }

    /* ADC calibration cleanup not needed for ESP32-C6 */

    g_pm_state.initialized = false;
}

/* Wake lock timer callback */
static void wake_lock_timer_callback(TimerHandle_t xTimer)
{
    /* Find which wake lock timer expired */
    for (int i = 0; i < WAKE_LOCK_COUNT; i++) {
        if (g_pm_state.wake_lock_timers[i] == xTimer) {
            ESP_LOGD(TAG, "Wake lock %d timer expired", i);
            xSemaphoreGive(g_pm_state.wake_locks[i]);
            break;
        }
    }
}

/* Acquire wake lock */
esp_err_t power_acquire_wake_lock(wake_lock_type_t type, uint32_t timeout_ms)
{
    if (!g_pm_state.initialized || type >= WAKE_LOCK_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(g_pm_state.mutex, portMAX_DELAY);

    /* Take the wake lock */
    if (xSemaphoreTake(g_pm_state.wake_locks[type], 0) == pdFALSE) {
        /* Wake lock already held */
        if (g_pm_state.debug_logging) {
            ESP_LOGD(TAG, "Wake lock %d already held", type);
        }
    } else {
        if (g_pm_state.debug_logging) {
            ESP_LOGI(TAG, "Acquired wake lock %d (timeout: %lu ms)", type, timeout_ms);
        }

        /* Acquire CPU frequency lock for active operations */
        esp_pm_lock_acquire(g_pm_state.pm_lock_cpu_freq_max);

        /* Update state to active */
        update_power_state(POWER_STATE_ACTIVE);
    }

    /* Set timeout if specified */
    if (timeout_ms > 0) {
        if (!g_pm_state.wake_lock_timers[type]) {
            g_pm_state.wake_lock_timers[type] = xTimerCreate("wake_lock_timer",
                                                            pdMS_TO_TICKS(timeout_ms),
                                                            pdFALSE,
                                                            (void*)(uintptr_t)type,
                                                            wake_lock_timer_callback);
        }
        if (g_pm_state.wake_lock_timers[type]) {
            xTimerChangePeriod(g_pm_state.wake_lock_timers[type], pdMS_TO_TICKS(timeout_ms), 0);
            xTimerStart(g_pm_state.wake_lock_timers[type], 0);
        }
    }

    /* Reset activity timer */
    xTimerReset(g_pm_state.activity_timer, 0);
    g_pm_state.stats.last_activity_time_ms = get_time_ms();

    xSemaphoreGive(g_pm_state.mutex);
    return ESP_OK;
}

/* Release wake lock */
esp_err_t power_release_wake_lock(wake_lock_type_t type)
{
    if (!g_pm_state.initialized || type >= WAKE_LOCK_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(g_pm_state.mutex, portMAX_DELAY);

    /* Stop timeout timer if running */
    if (g_pm_state.wake_lock_timers[type]) {
        xTimerStop(g_pm_state.wake_lock_timers[type], 0);
    }

    /* Release the wake lock */
    if (xSemaphoreGive(g_pm_state.wake_locks[type]) == pdTRUE) {
        if (g_pm_state.debug_logging) {
            ESP_LOGI(TAG, "Released wake lock %d", type);
        }

        /* Check if all locks are released */
        if (!power_has_wake_locks()) {
            /* Release CPU frequency lock */
            esp_pm_lock_release(g_pm_state.pm_lock_cpu_freq_max);
            
            /* Update state to idle */
            update_power_state(POWER_STATE_IDLE);
            
            /* Reset activity timer */
            xTimerReset(g_pm_state.activity_timer, 0);
        }
    }

    xSemaphoreGive(g_pm_state.mutex);
    return ESP_OK;
}

/* Check if any wake locks are held */
bool power_has_wake_locks(void)
{
    if (!g_pm_state.initialized) {
        return false;
    }

    for (int i = 0; i < WAKE_LOCK_COUNT; i++) {
        if (uxSemaphoreGetCount(g_pm_state.wake_locks[i]) == 0) {
            return true; /* Lock is held */
        }
    }
    return false;
}

/* Activity timer callback */
static void activity_timer_callback(TimerHandle_t xTimer)
{
    xSemaphoreTake(g_pm_state.mutex, portMAX_DELAY);

    if (!power_has_wake_locks() && g_pm_state.zigbee_can_sleep) {
        if (g_pm_state.debug_logging) {
            ESP_LOGI(TAG, "Activity timeout - entering sleep");
        }
        enter_light_sleep();
    }

    xSemaphoreGive(g_pm_state.mutex);
}

/* Update power state */
static void update_power_state(power_state_t new_state)
{
    if (g_pm_state.state != new_state) {
        uint32_t now = get_time_ms();
        
        /* Update statistics */
        switch (g_pm_state.state) {
        case POWER_STATE_ACTIVE:
            g_pm_state.stats.total_active_time_ms += (now - g_pm_state.stats.last_activity_time_ms);
            break;
        case POWER_STATE_SLEEPING:
            g_pm_state.stats.total_sleep_time_ms += (now - g_pm_state.stats.last_activity_time_ms);
            break;
        default:
            break;
        }

        g_pm_state.stats.last_activity_time_ms = now;
        g_pm_state.state = new_state;

        if (g_pm_state.debug_logging) {
            ESP_LOGI(TAG, "Power state: %s", 
                     new_state == POWER_STATE_ACTIVE ? "ACTIVE" :
                     new_state == POWER_STATE_IDLE ? "IDLE" : "SLEEPING");
        }
    }
}

/* Enter light sleep */
static void enter_light_sleep(void)
{
    update_power_state(POWER_STATE_SLEEPING);
    g_pm_state.stats.sleep_count++;

    /* Zigbee stack will handle the actual sleep entry */
    esp_zb_sleep_now();
}

/* Request immediate sleep */
esp_err_t power_request_sleep(void)
{
    if (!g_pm_state.initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    xSemaphoreTake(g_pm_state.mutex, portMAX_DELAY);

    if (!power_has_wake_locks() && g_pm_state.zigbee_can_sleep) {
        if (g_pm_state.debug_logging) {
            ESP_LOGI(TAG, "Sleep requested - entering sleep");
        }
        enter_light_sleep();
        xSemaphoreGive(g_pm_state.mutex);
        return ESP_OK;
    }

    xSemaphoreGive(g_pm_state.mutex);
    return ESP_ERR_INVALID_STATE;
}

/* Zigbee sleep allowed callback */
void power_on_zigbee_sleep_allowed(void)
{
    xSemaphoreTake(g_pm_state.mutex, portMAX_DELAY);
    
    g_pm_state.zigbee_can_sleep = true;
    
    if (!power_has_wake_locks()) {
        if (g_pm_state.debug_logging) {
            ESP_LOGI(TAG, "Zigbee sleep allowed - checking activity timeout");
        }
        /* Start or reset activity timer */
        xTimerReset(g_pm_state.activity_timer, 0);
    }
    
    xSemaphoreGive(g_pm_state.mutex);
}

/* Button wake callback */
void power_on_button_wake(void)
{
    if (!g_pm_state.initialized) {
        return;
    }

    g_pm_state.stats.wake_count++;
    update_power_state(POWER_STATE_ACTIVE);

    if (g_pm_state.debug_logging) {
        esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
        ESP_LOGI(TAG, "Wake from button (reason: %d)", wakeup_reason);
    }
}

/* Get current power state */
power_state_t power_get_state(void)
{
    return g_pm_state.state;
}

/* Get power statistics */
esp_err_t power_get_stats(power_stats_t *stats)
{
    if (!stats) {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(g_pm_state.mutex, portMAX_DELAY);
    memcpy(stats, &g_pm_state.stats, sizeof(power_stats_t));
    xSemaphoreGive(g_pm_state.mutex);

    return ESP_OK;
}

/* Reset power statistics */
void power_reset_stats(void)
{
    xSemaphoreTake(g_pm_state.mutex, portMAX_DELAY);
    
    uint32_t now = get_time_ms();
    memset(&g_pm_state.stats, 0, sizeof(power_stats_t));
    g_pm_state.stats.last_activity_time_ms = now;
    
    xSemaphoreGive(g_pm_state.mutex);
}

/* Battery monitoring timer callback */
static void battery_timer_callback(void* arg)
{
    if (!g_pm_state.config.battery_monitoring_enabled) {
        return;
    }

    /* Read ADC value */
    int adc_reading = adc1_get_raw(PM_BATTERY_ADC_CHANNEL);
    
    /* Convert to voltage (ESP32-C6 has 12-bit ADC, 0-3.3V range with 11dB attenuation) */
    /* Formula: voltage_mv = (adc_reading * 3300) / 4095 */
    uint32_t voltage = (adc_reading * 3300) / 4095;
    
    /* Account for voltage divider if present (adjust based on hardware) */
    /* Assuming a 2:1 voltage divider for 4.2V max battery */
    voltage *= 2;
    
    g_pm_state.stats.battery_voltage_mv = voltage;
    
    /* Calculate percentage (3.0V = 0%, 4.2V = 100%) */
    if (voltage >= 4200) {
        g_pm_state.stats.battery_percentage = 100;
    } else if (voltage <= 3000) {
        g_pm_state.stats.battery_percentage = 0;
    } else {
        g_pm_state.stats.battery_percentage = ((voltage - 3000) * 100) / (4200 - 3000);
    }

    if (g_pm_state.debug_logging) {
        ESP_LOGI(TAG, "Battery: %lu mV (%d%%)", voltage, g_pm_state.stats.battery_percentage);
    }

    /* Check for low battery */
    if (voltage < PM_BATTERY_LOW_MV) {
        ESP_LOGW(TAG, "Low battery: %lu mV", voltage);
    } else if (voltage < PM_BATTERY_CRITICAL_MV) {
        ESP_LOGE(TAG, "Critical battery: %lu mV", voltage);
    }
}

/* Get battery voltage */
uint32_t power_get_battery_voltage_mv(void)
{
    return g_pm_state.stats.battery_voltage_mv;
}

/* Get battery percentage */
uint8_t power_get_battery_percentage(void)
{
    return g_pm_state.stats.battery_percentage;
}

/* Set adaptive sleep parameters */
esp_err_t power_set_adaptive_sleep(uint32_t active_period_ms, uint32_t sleep_period_ms)
{
    if (!g_pm_state.initialized || !g_pm_state.config.adaptive_sleep_enabled) {
        return ESP_ERR_INVALID_STATE;
    }

    xSemaphoreTake(g_pm_state.mutex, portMAX_DELAY);

    /* Adjust activity timeout based on usage pattern */
    if (active_period_ms > 0 && sleep_period_ms > 0) {
        /* Calculate optimal timeout based on duty cycle */
        uint32_t total_period = active_period_ms + sleep_period_ms;
        uint32_t duty_cycle_percent = (active_period_ms * 100) / total_period;

        if (duty_cycle_percent < 10) {
            /* Very low activity - aggressive sleep */
            g_pm_state.config.activity_timeout_ms = 1000;
        } else if (duty_cycle_percent < 30) {
            /* Low activity - moderate sleep */
            g_pm_state.config.activity_timeout_ms = 5000;
        } else {
            /* Higher activity - less aggressive sleep */
            g_pm_state.config.activity_timeout_ms = 10000;
        }

        /* Update timer period */
        xTimerChangePeriod(g_pm_state.activity_timer, 
                          pdMS_TO_TICKS(g_pm_state.config.activity_timeout_ms), 0);

        if (g_pm_state.debug_logging) {
            ESP_LOGI(TAG, "Adaptive sleep: duty cycle %lu%%, timeout %lu ms",
                     duty_cycle_percent, g_pm_state.config.activity_timeout_ms);
        }
    }

    xSemaphoreGive(g_pm_state.mutex);
    return ESP_OK;
}

/* Enable debug logging */
void power_enable_debug_logging(bool enable)
{
    g_pm_state.debug_logging = enable;
    
    if (enable) {
        esp_log_level_set(TAG, ESP_LOG_DEBUG);
        ESP_LOGI(TAG, "Debug logging enabled");
    } else {
        esp_log_level_set(TAG, ESP_LOG_INFO);
    }
}

/* Get battery information */
esp_err_t power_get_battery_info(battery_info_t *info)
{
    if (!info) {
        return ESP_ERR_INVALID_ARG;
    }
    
    /* Initialize info structure */
    info->percentage = g_pm_state.stats.battery_percentage;
    info->voltage_mv = g_pm_state.stats.battery_voltage_mv;
    info->is_usb_connected = false; /* For battery-powered device, assume no USB */
    
    /* If battery monitoring is enabled, read current values */
    if (g_pm_state.config.battery_monitoring_enabled) {
        /* Read ADC value */
        int adc_reading = adc1_get_raw(PM_BATTERY_ADC_CHANNEL);
        
        /* Convert to voltage (ESP32-C6 has 12-bit ADC, 0-3.3V range with 12dB attenuation) */
        uint32_t voltage = (adc_reading * 3300) / 4095;
        
        /* Account for voltage divider if present (adjust based on hardware) */
        voltage *= 2;  /* Assuming 2:1 voltage divider */
        
        info->voltage_mv = voltage;
        
        /* Calculate percentage (3.0V = 0%, 4.2V = 100%) */
        if (voltage >= 4200) {
            info->percentage = 100;
        } else if (voltage <= 3000) {
            info->percentage = 0;
        } else {
            info->percentage = ((voltage - 3000) * 100) / (4200 - 3000);
        }
    }
    
    return ESP_OK;
}