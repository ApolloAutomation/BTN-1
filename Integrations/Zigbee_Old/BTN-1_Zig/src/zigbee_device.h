/*
 * BTN-1 Zigbee Device Configuration
 * Header file for device-specific Zigbee configuration
 */

#ifndef ZIGBEE_DEVICE_H
#define ZIGBEE_DEVICE_H

#include "esp_zigbee_core.h"
#include "button_handler.h"  /* For button_event_t */

/* Device configuration */
#define ESP_ZB_HA_ON_OFF_SWITCH_DEVICE_ID       0x0000  /* Generic On/Off switch */
#define BTN1_DEVICE_VERSION                      1
#define BTN1_ZIGBEE_PRIMARY_CHANNEL_MASK        ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK

/* Manufacturer and model information */
#define MANUFACTURER_NAME                        "Apollo Automation"
#define MODEL_IDENTIFIER                         "BTN-1"

/* Endpoint definitions for 4 buttons */
#define BUTTON_1_ENDPOINT                       1
#define BUTTON_2_ENDPOINT                       2
#define BUTTON_3_ENDPOINT                       3
#define BUTTON_4_ENDPOINT                       4
#define BTN1_ENDPOINT_COUNT                     4

/* Cluster IDs used by the device */
/* Server clusters (device provides these) */
#define ESP_ZB_ZCL_CLUSTER_ID_BASIC_SERVER      ESP_ZB_ZCL_CLUSTER_ID_BASIC
#define ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY_SERVER   ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY
#define ESP_ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG_SERVER ESP_ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG

/* Client clusters (device can control these on other devices) */
#define ESP_ZB_ZCL_CLUSTER_ID_ON_OFF_CLIENT    ESP_ZB_ZCL_CLUSTER_ID_ON_OFF
#define ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL_CLIENT ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL
#define ESP_ZB_ZCL_CLUSTER_ID_SCENES_CLIENT     ESP_ZB_ZCL_CLUSTER_ID_SCENES
#define ESP_ZB_ZCL_CLUSTER_ID_GROUPS_CLIENT     ESP_ZB_ZCL_CLUSTER_ID_GROUPS

/* Binding modes */
#define BTN1_BINDING_MODE_UNICAST    0
#define BTN1_BINDING_MODE_GROUP      1
#define BTN1_BINDING_MODE_BROADCAST  2

/* Function declarations */
esp_zb_ep_list_t *btn1_ep_create(void);
void btn1_configure_reporting(void);
void btn1_zigbee_init(void);

/* Button event handler - defined in zigbee_device.c */
/* Note: button_event_t is defined in button_handler.h */
void btn1_button_event_handler(button_event_t *event);

/* Binding management functions */
esp_err_t btn1_add_binding(uint8_t button, uint16_t addr, uint8_t endpoint, uint8_t mode);
esp_err_t btn1_remove_binding(uint8_t button);
esp_err_t btn1_clear_all_bindings(void);

/* Group management functions */
esp_err_t btn1_set_group(uint8_t button, uint16_t group_id);

/* Scene management functions */
esp_err_t btn1_set_scene(uint8_t button, uint8_t scene_id);

#endif /* ZIGBEE_DEVICE_H */