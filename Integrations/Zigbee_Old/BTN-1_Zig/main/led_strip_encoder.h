/*
 * LED Strip Encoder Configuration Header
 * For WS2812 addressable RGB LED control via RMT
 */

#ifndef LED_STRIP_ENCODER_H
#define LED_STRIP_ENCODER_H

#include "driver/rmt_encoder.h"

/* LED strip encoder configuration */
typedef struct {
    uint32_t resolution; /* Encoder resolution in Hz */
} led_strip_encoder_config_t;

#endif /* LED_STRIP_ENCODER_H */