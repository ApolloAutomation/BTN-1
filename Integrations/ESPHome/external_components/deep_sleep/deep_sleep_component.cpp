#include "deep_sleep_component.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

#ifdef USE_ESP8266
#include <Esp.h>
#endif

namespace esphome {
namespace deep_sleep {

static const char *const TAG = "deep_sleep";

bool global_has_deep_sleep = false;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void DeepSleepComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Deep Sleep...");
  global_has_deep_sleep = true;

  const optional<uint32_t> run_duration = get_run_duration_();
  if (run_duration.has_value()) {
    ESP_LOGI(TAG, "Scheduling Deep Sleep to start in %" PRIu32 " ms", *run_duration);
    this->set_timeout(*run_duration, [this]() { this->begin_sleep(); });
  } else {
    ESP_LOGD(TAG, "Not scheduling Deep Sleep, as no run duration is configured.");
  }
}

void DeepSleepComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up Deep Sleep...");
  if (this->sleep_duration_.has_value()) {
    uint32_t duration = *this->sleep_duration_ / 1000;
    ESP_LOGCONFIG(TAG, "  Sleep Duration: %" PRIu32 " ms", duration);
  }
  if (this->run_duration_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Run Duration: %" PRIu32 " ms", *this->run_duration_);
  }
  dump_config_platform_();
}

void DeepSleepComponent::loop() {
  if (this->next_enter_deep_sleep_) {
    if (this->prevent_) {
      this->next_enter_deep_sleep_ = false;
      return;
    }
    if (!prepare_to_sleep_()) {
      return;
    }

    this->deep_sleep_();
  }
}

float DeepSleepComponent::get_setup_priority() const { return setup_priority::LATE; }

float DeepSleepComponent::get_loop_priority() const { return -100.0f; }  // run after everything else is ready

void DeepSleepComponent::set_sleep_duration(uint32_t time_ms) { this->sleep_duration_ = uint64_t(time_ms) * 1000; }

void DeepSleepComponent::set_run_duration(uint32_t time_ms) { this->run_duration_ = time_ms; }

void DeepSleepComponent::begin_sleep(bool manual) {
  if (this->prevent_ && !manual) {
    this->next_enter_deep_sleep_ = true;
    return;
  }
  if (!prepare_to_sleep_())
    return;

  deep_sleep_();
}

void DeepSleepComponent::prevent_deep_sleep() { this->prevent_ = true; }

void DeepSleepComponent::allow_deep_sleep() { this->prevent_ = false; }

}  // namespace deep_sleep
}  // namespace esphome