/*
 * BTN-1 Power Management Module
 * Handles sleep modes, wake locks, and battery monitoring for ESP32-C6
 */

#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#include <stdint.h>
#include "esp_err.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* Power management configuration */
#define PM_MAX_CPU_FREQ             160         /* Maximum CPU frequency in MHz */
#define PM_MIN_CPU_FREQ             10          /* Minimum CPU frequency in MHz */
#define PM_LIGHT_SLEEP_ENABLE       true        /* Enable automatic light sleep */
#define PM_WAKE_LOCK_TIMEOUT_MS     5000        /* Default wake lock timeout */
#define PM_ACTIVITY_TIMEOUT_MS      30000       /* Activity timeout before deep sleep consideration */
#define PM_BATTERY_CHECK_INTERVAL_MS 60000      /* Battery voltage check interval */

/* Battery monitoring configuration */
#define PM_BATTERY_ADC_CHANNEL      ADC_CHANNEL_4  /* GPIO4 for battery monitoring */
#define PM_BATTERY_LOW_MV           2800           /* Low battery threshold (mV) */
#define PM_BATTERY_CRITICAL_MV      2500           /* Critical battery threshold (mV) */

/* Wake lock types */
typedef enum {
    WAKE_LOCK_BUTTON,       /* Wake lock for button processing */
    WAKE_LOCK_ZIGBEE,       /* Wake lock for Zigbee operations */
    WAKE_LOCK_SYSTEM,       /* General system wake lock */
    WAKE_LOCK_COUNT
} wake_lock_type_t;

/* Power state */
typedef enum {
    POWER_STATE_ACTIVE,     /* Device is active */
    POWER_STATE_IDLE,       /* Device is idle, ready for light sleep */
    POWER_STATE_SLEEPING,   /* Device is in light sleep */
} power_state_t;

/* Sleep statistics */
typedef struct {
    uint32_t total_active_time_ms;
    uint32_t total_sleep_time_ms;
    uint32_t sleep_count;
    uint32_t wake_count;
    uint32_t last_activity_time_ms;
    uint32_t battery_voltage_mv;
    uint8_t battery_percentage;
} power_stats_t;

/* Battery information structure */
typedef struct {
    uint8_t percentage;
    uint16_t voltage_mv;
    bool is_usb_connected;
} battery_info_t;

/* Power management configuration */
typedef struct {
    uint32_t activity_timeout_ms;       /* Time before entering sleep after last activity */
    uint32_t min_sleep_duration_ms;     /* Minimum sleep duration */
    bool adaptive_sleep_enabled;        /* Enable adaptive sleep based on usage */
    bool battery_monitoring_enabled;    /* Enable battery voltage monitoring */
} power_config_t;

/* Public API functions */

/**
 * @brief Initialize power management system
 * @param config Power management configuration (NULL for defaults)
 * @return ESP_OK on success
 */
esp_err_t power_management_init(const power_config_t *config);

/**
 * @brief Deinitialize power management system
 */
void power_management_deinit(void);

/**
 * @brief Acquire a wake lock to prevent sleep
 * @param type Wake lock type
 * @param timeout_ms Timeout in milliseconds (0 for infinite)
 * @return ESP_OK on success
 */
esp_err_t power_acquire_wake_lock(wake_lock_type_t type, uint32_t timeout_ms);

/**
 * @brief Release a wake lock
 * @param type Wake lock type
 * @return ESP_OK on success
 */
esp_err_t power_release_wake_lock(wake_lock_type_t type);

/**
 * @brief Check if any wake locks are held
 * @return true if wake locks are held
 */
bool power_has_wake_locks(void);

/**
 * @brief Request immediate sleep if no wake locks
 * @return ESP_OK if sleep was initiated
 */
esp_err_t power_request_sleep(void);

/**
 * @brief Get current power state
 * @return Current power state
 */
power_state_t power_get_state(void);

/**
 * @brief Get power statistics
 * @param stats Pointer to statistics structure
 * @return ESP_OK on success
 */
esp_err_t power_get_stats(power_stats_t *stats);

/**
 * @brief Reset power statistics
 */
void power_reset_stats(void);

/**
 * @brief Get battery voltage in millivolts
 * @return Battery voltage in mV, or 0 if not available
 */
uint32_t power_get_battery_voltage_mv(void);

/**
 * @brief Get battery percentage (0-100)
 * @return Battery percentage, or 0 if not available
 */
uint8_t power_get_battery_percentage(void);

/**
 * @brief Set adaptive sleep parameters based on usage
 * @param active_period_ms Expected active period in ms
 * @param sleep_period_ms Expected sleep period in ms
 * @return ESP_OK on success
 */
esp_err_t power_set_adaptive_sleep(uint32_t active_period_ms, uint32_t sleep_period_ms);

/**
 * @brief Enable or disable power consumption logging
 * @param enable true to enable logging
 */
void power_enable_debug_logging(bool enable);

/**
 * @brief Callback for Zigbee sleep allowed signal
 */
void power_on_zigbee_sleep_allowed(void);

/**
 * @brief Callback for button wake-up event
 */
void power_on_button_wake(void);

/**
 * @brief Get battery information
 * @param info Pointer to battery_info_t structure to fill
 * @return ESP_OK on success
 */
esp_err_t power_get_battery_info(battery_info_t *info);

#endif /* POWER_MANAGEMENT_H */