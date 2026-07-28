// Runtime poll interval control for the Apollo BTN-1 Zigbee firmware.
//
// A sleepy end device fetches buffered commands when it polls its parent.
// The ZBOSS long poll interval can be changed at runtime, which is how the
// Instant Response switch trades battery for latency while on USB power.
// Call only after the Zigbee stack is running (guard on is_connected()).

#pragma once

#include "freertos/FreeRTOS.h"
#include "esp_zigbee_core.h"

extern "C" void zb_zdo_pim_set_long_poll_interval(uint32_t ms);

inline void apollo_set_poll_interval(uint32_t ms) {
  if (esp_zb_lock_acquire(100 / portTICK_PERIOD_MS)) {
    zb_zdo_pim_set_long_poll_interval(ms);
    esp_zb_lock_release();
  }
}
