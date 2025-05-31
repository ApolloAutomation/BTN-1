#ifdef USE_ESP8266
#include "deep_sleep_component.h"
#include "esphome/core/log.h"

#include <Esp.h>

namespace esphome {
namespace deep_sleep {

static const char *const TAG = "deep_sleep";

optional<uint32_t> DeepSleepComponent::get_run_duration_() const { return this->run_duration_; }

void DeepSleepComponent::dump_config_platform_() {}

bool DeepSleepComponent::prepare_to_sleep_() { return true; }

void DeepSleepComponent::deep_sleep_() {
  ESP_LOGD(TAG, "Beginning Deep Sleep");
  if (this->sleep_duration_.has_value()) {
    ESP_LOGD(TAG, "Sleeping for %" PRId64 "us", *this->sleep_duration_);
    ESP.deepSleep(*this->sleep_duration_);
  } else {
    ESP.deepSleep(0);
  }
}

}  // namespace deep_sleep
}  // namespace esphome
#endif