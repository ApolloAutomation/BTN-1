// Auto generated code by esphome
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphome.h"
using namespace esphome;
using std::isnan;
using std::min;
using std::max;
using namespace number;
using namespace binary_sensor;
using namespace sensor;
using namespace switch_;
using namespace light;
using namespace button;
logger::Logger *logger_logger_id;
using namespace i2c;
i2c::IDFI2CBus *i2c_bus;
zigbee::ZigBeeComponent *zb;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id;
using namespace output;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id;
ledc::LEDCOutput *buzzer;
rtttl::Rtttl *rtttl_buzzer;
deep_sleep::DeepSleepComponent *deep_sleep_1;
template_::TemplateNumber *deep_sleep_sleep_duration;
number::NumberStateTrigger *number_numberstatetrigger_id;
Automation<float> *automation_id_2;
StatelessLambdaAction<float> *lambdaaction_id_2;
gpio::GPIOBinarySensor *reset_button;
binary_sensor::PressTrigger *binary_sensor_presstrigger_id;
Automation<> *automation_id_3;
StatelessLambdaAction<> *lambdaaction_id_3;
gpio::GPIOBinarySensor *button_1;
binary_sensor::PressTrigger *binary_sensor_presstrigger_id_2;
Automation<> *automation_id_6;
StatelessLambdaAction<> *lambdaaction_id_7;
zigbee::ReportAction<> *zigbee_reportaction_id;
binary_sensor::ReleaseTrigger *binary_sensor_releasetrigger_id_2;
Automation<> *automation_id_7;
StatelessLambdaAction<> *lambdaaction_id_8;
zigbee::ReportAction<> *zigbee_reportaction_id_2;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_7;
gpio::GPIOBinarySensor *button_2;
binary_sensor::PressTrigger *binary_sensor_presstrigger_id_3;
Automation<> *automation_id_8;
StatelessLambdaAction<> *lambdaaction_id_9;
zigbee::ReportAction<> *zigbee_reportaction_id_3;
binary_sensor::ReleaseTrigger *binary_sensor_releasetrigger_id_3;
Automation<> *automation_id_9;
StatelessLambdaAction<> *lambdaaction_id_10;
zigbee::ReportAction<> *zigbee_reportaction_id_4;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_8;
gpio::GPIOBinarySensor *button_3;
binary_sensor::PressTrigger *binary_sensor_presstrigger_id_4;
Automation<> *automation_id_10;
StatelessLambdaAction<> *lambdaaction_id_11;
zigbee::ReportAction<> *zigbee_reportaction_id_5;
binary_sensor::ReleaseTrigger *binary_sensor_releasetrigger_id_4;
Automation<> *automation_id_11;
StatelessLambdaAction<> *lambdaaction_id_12;
zigbee::ReportAction<> *zigbee_reportaction_id_6;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_9;
gpio::GPIOBinarySensor *button_4;
binary_sensor::PressTrigger *binary_sensor_presstrigger_id_5;
Automation<> *automation_id_12;
StatelessLambdaAction<> *lambdaaction_id_13;
zigbee::ReportAction<> *zigbee_reportaction_id_7;
binary_sensor::ReleaseTrigger *binary_sensor_releasetrigger_id_5;
Automation<> *automation_id_13;
StatelessLambdaAction<> *lambdaaction_id_14;
zigbee::ReportAction<> *zigbee_reportaction_id_8;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_10;
max17048::MAX17048Component *max_17048;
sensor::Sensor *batt_v;
sensor::Sensor *batt_pct;
sensor::StatelessLambdaFilter *sensor_lambdafilter_id;
template_::TemplateSwitch *prevent_sleep;
switch_::SwitchTurnOnTrigger *switch__switchturnontrigger_id;
Automation<> *automation_id_14;
StatelessLambdaAction<> *lambdaaction_id_15;
switch_::SwitchTurnOffTrigger *switch__switchturnofftrigger_id;
Automation<> *automation_id_15;
StatelessLambdaAction<> *lambdaaction_id_16;
factory_reset::FactoryResetSwitch *factory_reset_switch;
gpio::GPIOSwitch *power_latch;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_11;
gpio::GPIOSwitch *accessory_power;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_12;
esp32_rmt_led_strip::ESP32RMTLEDStripLightOutput *esp32_rmt_led_strip_esp32rmtledstriplightoutput_id;
light::AddressableLightState *rgb_light;
light::PulseLightEffect *light_pulselighteffect_id;
light::PulseLightEffect *light_pulselighteffect_id_2;
partition::PartitionLightOutput *partition_partitionlightoutput_id;
light::AddressableLightState *btn_1_light;
light::PulseLightEffect *light_pulselighteffect_id_3;
light::PulseLightEffect *light_pulselighteffect_id_4;
partition::PartitionLightOutput *partition_partitionlightoutput_id_2;
light::AddressableLightState *btn_2_light;
light::PulseLightEffect *light_pulselighteffect_id_5;
light::PulseLightEffect *light_pulselighteffect_id_6;
partition::PartitionLightOutput *partition_partitionlightoutput_id_3;
light::AddressableLightState *btn_3_light;
light::PulseLightEffect *light_pulselighteffect_id_7;
light::PulseLightEffect *light_pulselighteffect_id_8;
partition::PartitionLightOutput *partition_partitionlightoutput_id_4;
light::AddressableLightState *btn_4_light;
light::PulseLightEffect *light_pulselighteffect_id_9;
light::PulseLightEffect *light_pulselighteffect_id_10;
restart::RestartButton *restart_restartbutton_id;
preferences::IntervalSyncer *preferences_intervalsyncer_id;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_2;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_3;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_4;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_5;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_6;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_7;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_8;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_9;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_10;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_11;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_12;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_13;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_14;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_15;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_16;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_17;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_18;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_19;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_20;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_21;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_22;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_23;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_24;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_25;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_26;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_27;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_28;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_29;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_30;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_31;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_32;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_33;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_34;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_35;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_36;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_37;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_38;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_39;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_40;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_41;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_42;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_43;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_44;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_45;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_46;
zigbee::ZigBeeAttribute *zigbee_zigbeeattribute_id_47;
zigbee::ZigBeeJoinTrigger *zigbee_zigbeejointrigger_id;
Automation<> *automation_id;
StatelessLambdaAction<> *lambdaaction_id;
globals::GlobalsComponent<int> *cycleCounter;
globals::GlobalsComponent<uint32_t> *button_press_timestamp;
StatelessLambdaAction<> *lambdaaction_id_4;
binary_sensor::ReleaseTrigger *binary_sensor_releasetrigger_id;
Automation<> *automation_id_4;
StatelessLambdaAction<> *lambdaaction_id_5;
binary_sensor::ClickTrigger *binary_sensor_clicktrigger_id;
Automation<> *automation_id_5;
StatelessLambdaAction<> *lambdaaction_id_6;
zigbee::ResetZigbeeAction<> *zigbee_resetzigbeeaction_id;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_6;
// ========== AUTO GENERATED INCLUDE BLOCK END ==========="

void setup() {
  // ========== AUTO GENERATED CODE BEGIN ===========
  // esphome:
  //   name: apollo-btn-1-zb
  //   friendly_name: Apollo BTN-1 Zigbee
  //   min_version: 2025.11.4
  //   build_path: build/apollo-btn-1-zb
  //   platformio_options: {}
  //   environment_variables: {}
  //   includes: []
  //   includes_c: []
  //   libraries: []
  //   name_add_mac_suffix: false
  //   debug_scheduler: false
  //   areas: []
  //   devices: []
  App.pre_setup("apollo-btn-1-zb", "Apollo BTN-1 Zigbee", "", __DATE__ ", " __TIME__, false);
  // number:
  // binary_sensor:
  // sensor:
  // switch:
  // light:
  // button:
  // logger:
  //   level: INFO
  //   baud_rate: 115200
  //   id: logger_logger_id
  //   tx_buffer_size: 512
  //   deassert_rts_dtr: false
  //   task_log_buffer_size: 768
  //   hardware_uart: USB_SERIAL_JTAG
  //   logs: {}
  //   runtime_tag_levels: false
  logger_logger_id = new logger::Logger(115200, 512);
  logger_logger_id->create_pthread_key();
  logger_logger_id->init_log_buffer(768);
  logger_logger_id->set_log_level(ESPHOME_LOG_LEVEL_INFO);
  logger_logger_id->set_uart_selection(logger::UART_SELECTION_USB_SERIAL_JTAG);
  logger_logger_id->pre_setup();
  logger_logger_id->set_component_source(LOG_STR("logger"));
  App.register_component(logger_logger_id);
  // i2c:
  //   id: i2c_bus
  //   sda: 1
  //   scl: 0
  //   sda_pullup_enabled: true
  //   scl_pullup_enabled: true
  //   frequency: 50000.0
  //   scan: true
  i2c_bus = new i2c::IDFI2CBus();
  i2c_bus->set_component_source(LOG_STR("i2c"));
  App.register_component(i2c_bus);
  i2c_bus->set_sda_pin(1);
  i2c_bus->set_sda_pullup_enabled(true);
  i2c_bus->set_scl_pin(0);
  i2c_bus->set_scl_pullup_enabled(true);
  i2c_bus->set_frequency(50000);
  i2c_bus->set_scan(true);
  // esp32:
  //   board: esp32-c6-devkitm-1
  //   variant: ESP32C6
  //   flash_size: 8MB
  //   partitions: /Users/justinapollo/Code/ApolloAutomation/BTN-1/Integrations/ESPHome_Zigbee/examples/../partitions/partitions_zb.csv
  //   framework:
  //     type: esp-idf
  //     sdkconfig_options:
  //       CONFIG_ZB_ENABLED: y
  //       CONFIG_ZB_ZCZR: y
  //       CONFIG_ZB_ZED: n
  //     version: 5.5.1
  //     log_level: ERROR
  //     advanced:
  //       compiler_optimization: SIZE
  //       enable_idf_experimental_features: false
  //       enable_lwip_assert: true
  //       ignore_efuse_custom_mac: false
  //       ignore_efuse_mac_crc: false
  //       enable_lwip_dhcp_server: false
  //       enable_lwip_mdns_queries: true
  //       enable_lwip_bridge_interface: false
  //       enable_lwip_tcpip_core_locking: true
  //       enable_lwip_check_thread_safety: true
  //       disable_libc_locks_in_iram: true
  //       disable_vfs_support_termios: true
  //       disable_vfs_support_select: true
  //       disable_vfs_support_dir: true
  //       execute_from_psram: false
  //       loop_task_stack_size: 8192
  //     components: []
  //     platform_version: https:github.com/pioarduino/platform-espressif32/releases/download/55.03.31-2/platform-espressif32.zip
  //     source: pioarduino/framework-espidf@https:github.com/pioarduino/esp-idf/releases/download/v5.5.1/esp-idf-v5.5.1.tar.xz
  //   cpu_frequency: 160MHZ
  // external_components:
  //   - source:
  //       url: https:github.com/luar123/zigbee_esphome.git
  //       type: git
  //     components:
  //       - zigbee
  //     refresh: 1d
  //   - source:
  //       url: https:github.com/ApolloAutomation/esphome-battery-component.git
  //       type: git
  //     components:
  //       - max17048
  //     refresh: 1d
  // zigbee:
  //   id: zb
  //   components: all
  //   manufacturer: Apollo Automation
  //   name: BTN-1
  //   on_join:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: '*** ZIGBEE NETWORK JOINED ***'
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id
  //       automation_id: automation_id
  //       trigger_id: zigbee_zigbeejointrigger_id
  //   date: '20260102'
  //   power_supply: 0
  //   version: 0
  //   area: 0
  //   router: false
  //   debug: false
  //   as_generic: false
  zb = new zigbee::ZigBeeComponent();
  zb->set_component_source(LOG_STR("zigbee"));
  App.register_component(zb);
  zb->set_basic_cluster("BTN-1", "Apollo Automation", "20260102", 0, 0, 0, 0, "", 0);
  zb->create_default_cluster(1, ::ESP_ZB_HA_COLOR_DIMMABLE_LIGHT_DEVICE_ID);
  zb->add_cluster(1, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(1, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id = new zigbee::ZigBeeAttribute(zb, 1, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id);
  zigbee_zigbeeattribute_id->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id->set_report(false);
  // output:
  // output.ledc:
  //   platform: ledc
  //   pin:
  //     number: 23
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   id: buzzer
  //   zero_means_zero: false
  //   frequency: 1000.0
  esp32_esp32internalgpiopin_id = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id->set_pin(::GPIO_NUM_23);
  esp32_esp32internalgpiopin_id->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id->set_flags(gpio::Flags::FLAG_OUTPUT);
  buzzer = new ledc::LEDCOutput(esp32_esp32internalgpiopin_id);
  buzzer->set_component_source(LOG_STR("ledc.output"));
  App.register_component(buzzer);
  buzzer->set_zero_means_zero(false);
  buzzer->set_frequency(1000.0f);
  // rtttl:
  //   id: rtttl_buzzer
  //   output: buzzer
  //   gain: 0.6
  rtttl_buzzer = new rtttl::Rtttl();
  rtttl_buzzer->set_component_source(LOG_STR("rtttl"));
  App.register_component(rtttl_buzzer);
  rtttl_buzzer->set_output(buzzer);
  rtttl_buzzer->set_gain(0.6f);
  // deep_sleep:
  //   id: deep_sleep_1
  //   sleep_duration: 24h
  //   run_duration: 90s
  //   esp32_ext1_wakeup:
  //     mode: ANY_HIGH
  //     pins:
  //       - number: 2
  //         allow_other_uses: true
  //         mode:
  //           input: true
  //           output: false
  //           open_drain: false
  //           pullup: false
  //           pulldown: false
  //         id: esp32_esp32internalgpiopin_id_2
  //         inverted: false
  //         ignore_pin_validation_error: false
  //         ignore_strapping_warning: false
  //         drive_strength: 20.0
  //       - number: 4
  //         allow_other_uses: true
  //         mode:
  //           input: true
  //           output: false
  //           open_drain: false
  //           pullup: false
  //           pulldown: false
  //         id: esp32_esp32internalgpiopin_id_3
  //         inverted: false
  //         ignore_pin_validation_error: false
  //         ignore_strapping_warning: false
  //         drive_strength: 20.0
  //       - number: 5
  //         allow_other_uses: true
  //         mode:
  //           input: true
  //           output: false
  //           open_drain: false
  //           pullup: false
  //           pulldown: false
  //         id: esp32_esp32internalgpiopin_id_4
  //         inverted: false
  //         ignore_pin_validation_error: false
  //         ignore_strapping_warning: false
  //         drive_strength: 20.0
  //       - number: 6
  //         allow_other_uses: true
  //         mode:
  //           input: true
  //           output: false
  //           open_drain: false
  //           pullup: false
  //           pulldown: false
  //         id: esp32_esp32internalgpiopin_id_5
  //         inverted: false
  //         ignore_pin_validation_error: false
  //         ignore_strapping_warning: false
  //         drive_strength: 20.0
  deep_sleep_1 = new deep_sleep::DeepSleepComponent();
  deep_sleep_1->set_component_source(LOG_STR("deep_sleep"));
  App.register_component(deep_sleep_1);
  deep_sleep_1->set_sleep_duration(86400000);
  deep_sleep_1->set_run_duration(90000);
  deep_sleep_1->set_ext1_wakeup(deep_sleep::Ext1Wakeup{
      .mask = 116,
      .wakeup_mode = ::ESP_EXT1_WAKEUP_ANY_HIGH,
  });
  // number.template:
  //   platform: template
  //   name: Sleep Duration
  //   id: deep_sleep_sleep_duration
  //   min_value: 0.0
  //   max_value: 800.0
  //   step: 1.0
  //   mode: BOX
  //   update_interval: 4294967295
  //   optimistic: true
  //   restore_value: true
  //   initial_value: 24.0
  //   icon: mdi:arrow-collapse-right
  //   entity_category: config
  //   unit_of_measurement: h
  //   on_value:
  //     - then:
  //         - lambda: !lambda |-
  //             id(deep_sleep_1).set_sleep_duration(x * 60 * 60 * 1000);
  //           type_id: lambdaaction_id_2
  //       automation_id: automation_id_2
  //       trigger_id: number_numberstatetrigger_id
  //   disabled_by_default: false
  deep_sleep_sleep_duration = new template_::TemplateNumber();
  deep_sleep_sleep_duration->set_update_interval(4294967295UL);
  deep_sleep_sleep_duration->set_component_source(LOG_STR("template.number"));
  App.register_component(deep_sleep_sleep_duration);
  App.register_number(deep_sleep_sleep_duration);
  deep_sleep_sleep_duration->set_name("Sleep Duration");
  deep_sleep_sleep_duration->set_object_id("sleep_duration");
  deep_sleep_sleep_duration->set_icon("mdi:arrow-collapse-right");
  deep_sleep_sleep_duration->set_entity_category(::ENTITY_CATEGORY_CONFIG);
  deep_sleep_sleep_duration->traits.set_min_value(0.0f);
  deep_sleep_sleep_duration->traits.set_max_value(800.0f);
  deep_sleep_sleep_duration->traits.set_step(1.0f);
  deep_sleep_sleep_duration->traits.set_mode(number::NUMBER_MODE_BOX);
  number_numberstatetrigger_id = new number::NumberStateTrigger(deep_sleep_sleep_duration);
  automation_id_2 = new Automation<float>(number_numberstatetrigger_id);
  lambdaaction_id_2 = new StatelessLambdaAction<float>([](float x) -> void {
      #line 158 "btn1_zigbee_simple_FIXED.yaml"
      deep_sleep_1->set_sleep_duration(x * 60 * 60 * 1000);
  });
  automation_id_2->add_actions({lambdaaction_id_2});
  deep_sleep_sleep_duration->traits.set_unit_of_measurement("h");
  deep_sleep_sleep_duration->set_optimistic(true);
  deep_sleep_sleep_duration->set_initial_value(24.0f);
  deep_sleep_sleep_duration->set_restore_value(true);
  // binary_sensor.gpio:
  //   platform: gpio
  //   pin:
  //     number: 9
  //     inverted: true
  //     mode:
  //       input: true
  //       pullup: true
  //       output: false
  //       open_drain: false
  //       pulldown: false
  //     ignore_strapping_warning: true
  //     id: esp32_esp32internalgpiopin_id_6
  //     ignore_pin_validation_error: false
  //     drive_strength: 20.0
  //   id: reset_button
  //   name: Reset Button
  //   internal: true
  //   on_press:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Reset button pressed - hold 5-10s to reset Zigbee
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_3
  //         - lambda: !lambda |-
  //             id(button_press_timestamp) = millis();
  //           type_id: lambdaaction_id_4
  //       automation_id: automation_id_3
  //       trigger_id: binary_sensor_presstrigger_id
  //   on_release:
  //     - then:
  //         - lambda: !lambda |-
  //             uint32_t hold_time = millis() - id(button_press_timestamp);
  //             ESP_LOGI("button", "Reset button released after %d ms", hold_time);
  //   
  //             if (hold_time >= 10000) {
  //                10+ seconds: Factory reset
  //               ESP_LOGI("button", "Factory reset triggered!");
  //               id(prevent_sleep).turn_on();
  //               id(factory_reset_switch).turn_on();
  //             }
  //             else if (hold_time >= 3000) {
  //                3-5 seconds: Prevent sleep
  //               ESP_LOGI("button", "Prevent sleep activated");
  //               id(prevent_sleep).turn_on();
  //               id(deep_sleep_1).prevent_deep_sleep();
  //             }
  //           type_id: lambdaaction_id_5
  //       automation_id: automation_id_4
  //       trigger_id: binary_sensor_releasetrigger_id
  //   on_click:
  //     - min_length: 5s
  //       max_length: 10s
  //       then:
  //         - logger.log:
  //             level: INFO
  //             format: '*** ZIGBEE RESET - Entering pairing mode ***'
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_6
  //         - zigbee.reset:
  //             id: zb
  //           type_id: zigbee_resetzigbeeaction_id
  //       automation_id: automation_id_5
  //       trigger_id: binary_sensor_clicktrigger_id
  //   disabled_by_default: false
  //   use_interrupt: true
  //   interrupt_type: ANY
  reset_button = new gpio::GPIOBinarySensor();
  App.register_binary_sensor(reset_button);
  reset_button->set_name("Reset Button");
  reset_button->set_object_id("reset_button");
  reset_button->set_internal(true);
  reset_button->set_trigger_on_initial_state(false);
  binary_sensor_presstrigger_id = new binary_sensor::PressTrigger(reset_button);
  automation_id_3 = new Automation<>(binary_sensor_presstrigger_id);
  lambdaaction_id_3 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Reset button pressed - hold 5-10s to reset Zigbee");
  });
  // binary_sensor.gpio:
  //   platform: gpio
  //   id: button_1
  //   name: Button 1
  //   pin:
  //     number: 2
  //     allow_other_uses: true
  //     mode:
  //       input: true
  //       pulldown: true
  //       output: false
  //       open_drain: false
  //       pullup: false
  //     id: esp32_esp32internalgpiopin_id_7
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   on_press:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Button 1 PRESSED - sending Zigbee report
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_7
  //         - zigbee.report:
  //             id: zb
  //           type_id: zigbee_reportaction_id
  //       automation_id: automation_id_6
  //       trigger_id: binary_sensor_presstrigger_id_2
  //   on_release:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Button 1 RELEASED - sending Zigbee report
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_8
  //         - zigbee.report:
  //             id: zb
  //           type_id: zigbee_reportaction_id_2
  //       automation_id: automation_id_7
  //       trigger_id: binary_sensor_releasetrigger_id_2
  //   disabled_by_default: false
  //   use_interrupt: true
  //   interrupt_type: ANY
  button_1 = new gpio::GPIOBinarySensor();
  App.register_binary_sensor(button_1);
  button_1->set_name("Button 1");
  button_1->set_object_id("button_1");
  button_1->set_trigger_on_initial_state(false);
  binary_sensor_presstrigger_id_2 = new binary_sensor::PressTrigger(button_1);
  automation_id_6 = new Automation<>(binary_sensor_presstrigger_id_2);
  lambdaaction_id_7 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Button 1 PRESSED - sending Zigbee report");
  });
  zigbee_reportaction_id = new zigbee::ReportAction<>();
  zigbee_reportaction_id->set_parent(zb);
  automation_id_6->add_actions({lambdaaction_id_7, zigbee_reportaction_id});
  binary_sensor_releasetrigger_id_2 = new binary_sensor::ReleaseTrigger(button_1);
  automation_id_7 = new Automation<>(binary_sensor_releasetrigger_id_2);
  lambdaaction_id_8 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Button 1 RELEASED - sending Zigbee report");
  });
  zigbee_reportaction_id_2 = new zigbee::ReportAction<>();
  zigbee_reportaction_id_2->set_parent(zb);
  automation_id_7->add_actions({lambdaaction_id_8, zigbee_reportaction_id_2});
  button_1->set_component_source(LOG_STR("gpio.binary_sensor"));
  App.register_component(button_1);
  esp32_esp32internalgpiopin_id_7 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_7->set_pin(::GPIO_NUM_2);
  esp32_esp32internalgpiopin_id_7->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_7->set_flags((gpio::Flags::FLAG_INPUT | gpio::Flags::FLAG_PULLDOWN));
  button_1->set_pin(esp32_esp32internalgpiopin_id_7);
  button_1->set_use_interrupt(false);
  // binary_sensor.gpio:
  //   platform: gpio
  //   id: button_2
  //   name: Button 2
  //   pin:
  //     number: 4
  //     allow_other_uses: true
  //     mode:
  //       input: true
  //       pulldown: true
  //       output: false
  //       open_drain: false
  //       pullup: false
  //     id: esp32_esp32internalgpiopin_id_8
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   on_press:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Button 2 PRESSED - sending Zigbee report
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_9
  //         - zigbee.report:
  //             id: zb
  //           type_id: zigbee_reportaction_id_3
  //       automation_id: automation_id_8
  //       trigger_id: binary_sensor_presstrigger_id_3
  //   on_release:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Button 2 RELEASED - sending Zigbee report
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_10
  //         - zigbee.report:
  //             id: zb
  //           type_id: zigbee_reportaction_id_4
  //       automation_id: automation_id_9
  //       trigger_id: binary_sensor_releasetrigger_id_3
  //   disabled_by_default: false
  //   use_interrupt: true
  //   interrupt_type: ANY
  button_2 = new gpio::GPIOBinarySensor();
  App.register_binary_sensor(button_2);
  button_2->set_name("Button 2");
  button_2->set_object_id("button_2");
  button_2->set_trigger_on_initial_state(false);
  binary_sensor_presstrigger_id_3 = new binary_sensor::PressTrigger(button_2);
  automation_id_8 = new Automation<>(binary_sensor_presstrigger_id_3);
  lambdaaction_id_9 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Button 2 PRESSED - sending Zigbee report");
  });
  zigbee_reportaction_id_3 = new zigbee::ReportAction<>();
  zigbee_reportaction_id_3->set_parent(zb);
  automation_id_8->add_actions({lambdaaction_id_9, zigbee_reportaction_id_3});
  binary_sensor_releasetrigger_id_3 = new binary_sensor::ReleaseTrigger(button_2);
  automation_id_9 = new Automation<>(binary_sensor_releasetrigger_id_3);
  lambdaaction_id_10 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Button 2 RELEASED - sending Zigbee report");
  });
  zigbee_reportaction_id_4 = new zigbee::ReportAction<>();
  zigbee_reportaction_id_4->set_parent(zb);
  automation_id_9->add_actions({lambdaaction_id_10, zigbee_reportaction_id_4});
  button_2->set_component_source(LOG_STR("gpio.binary_sensor"));
  App.register_component(button_2);
  esp32_esp32internalgpiopin_id_8 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_8->set_pin(::GPIO_NUM_4);
  esp32_esp32internalgpiopin_id_8->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_8->set_flags((gpio::Flags::FLAG_INPUT | gpio::Flags::FLAG_PULLDOWN));
  button_2->set_pin(esp32_esp32internalgpiopin_id_8);
  button_2->set_use_interrupt(false);
  // binary_sensor.gpio:
  //   platform: gpio
  //   id: button_3
  //   name: Button 3
  //   pin:
  //     number: 6
  //     allow_other_uses: true
  //     mode:
  //       input: true
  //       pulldown: true
  //       output: false
  //       open_drain: false
  //       pullup: false
  //     id: esp32_esp32internalgpiopin_id_9
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   on_press:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Button 3 PRESSED - sending Zigbee report
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_11
  //         - zigbee.report:
  //             id: zb
  //           type_id: zigbee_reportaction_id_5
  //       automation_id: automation_id_10
  //       trigger_id: binary_sensor_presstrigger_id_4
  //   on_release:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Button 3 RELEASED - sending Zigbee report
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_12
  //         - zigbee.report:
  //             id: zb
  //           type_id: zigbee_reportaction_id_6
  //       automation_id: automation_id_11
  //       trigger_id: binary_sensor_releasetrigger_id_4
  //   disabled_by_default: false
  //   use_interrupt: true
  //   interrupt_type: ANY
  button_3 = new gpio::GPIOBinarySensor();
  App.register_binary_sensor(button_3);
  button_3->set_name("Button 3");
  button_3->set_object_id("button_3");
  button_3->set_trigger_on_initial_state(false);
  binary_sensor_presstrigger_id_4 = new binary_sensor::PressTrigger(button_3);
  automation_id_10 = new Automation<>(binary_sensor_presstrigger_id_4);
  lambdaaction_id_11 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Button 3 PRESSED - sending Zigbee report");
  });
  zigbee_reportaction_id_5 = new zigbee::ReportAction<>();
  zigbee_reportaction_id_5->set_parent(zb);
  automation_id_10->add_actions({lambdaaction_id_11, zigbee_reportaction_id_5});
  binary_sensor_releasetrigger_id_4 = new binary_sensor::ReleaseTrigger(button_3);
  automation_id_11 = new Automation<>(binary_sensor_releasetrigger_id_4);
  lambdaaction_id_12 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Button 3 RELEASED - sending Zigbee report");
  });
  zigbee_reportaction_id_6 = new zigbee::ReportAction<>();
  zigbee_reportaction_id_6->set_parent(zb);
  automation_id_11->add_actions({lambdaaction_id_12, zigbee_reportaction_id_6});
  button_3->set_component_source(LOG_STR("gpio.binary_sensor"));
  App.register_component(button_3);
  esp32_esp32internalgpiopin_id_9 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_9->set_pin(::GPIO_NUM_6);
  esp32_esp32internalgpiopin_id_9->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_9->set_flags((gpio::Flags::FLAG_INPUT | gpio::Flags::FLAG_PULLDOWN));
  button_3->set_pin(esp32_esp32internalgpiopin_id_9);
  button_3->set_use_interrupt(false);
  // binary_sensor.gpio:
  //   platform: gpio
  //   id: button_4
  //   name: Button 4
  //   pin:
  //     number: 5
  //     allow_other_uses: true
  //     mode:
  //       input: true
  //       pulldown: true
  //       output: false
  //       open_drain: false
  //       pullup: false
  //     id: esp32_esp32internalgpiopin_id_10
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   on_press:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Button 4 PRESSED - sending Zigbee report
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_13
  //         - zigbee.report:
  //             id: zb
  //           type_id: zigbee_reportaction_id_7
  //       automation_id: automation_id_12
  //       trigger_id: binary_sensor_presstrigger_id_5
  //   on_release:
  //     - then:
  //         - logger.log:
  //             level: INFO
  //             format: Button 4 RELEASED - sending Zigbee report
  //             args: []
  //             tag: main
  //             logger_id: logger_logger_id
  //           type_id: lambdaaction_id_14
  //         - zigbee.report:
  //             id: zb
  //           type_id: zigbee_reportaction_id_8
  //       automation_id: automation_id_13
  //       trigger_id: binary_sensor_releasetrigger_id_5
  //   disabled_by_default: false
  //   use_interrupt: true
  //   interrupt_type: ANY
  button_4 = new gpio::GPIOBinarySensor();
  App.register_binary_sensor(button_4);
  button_4->set_name("Button 4");
  button_4->set_object_id("button_4");
  button_4->set_trigger_on_initial_state(false);
  binary_sensor_presstrigger_id_5 = new binary_sensor::PressTrigger(button_4);
  automation_id_12 = new Automation<>(binary_sensor_presstrigger_id_5);
  lambdaaction_id_13 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Button 4 PRESSED - sending Zigbee report");
  });
  zigbee_reportaction_id_7 = new zigbee::ReportAction<>();
  zigbee_reportaction_id_7->set_parent(zb);
  automation_id_12->add_actions({lambdaaction_id_13, zigbee_reportaction_id_7});
  binary_sensor_releasetrigger_id_5 = new binary_sensor::ReleaseTrigger(button_4);
  automation_id_13 = new Automation<>(binary_sensor_releasetrigger_id_5);
  lambdaaction_id_14 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "Button 4 RELEASED - sending Zigbee report");
  });
  zigbee_reportaction_id_8 = new zigbee::ReportAction<>();
  zigbee_reportaction_id_8->set_parent(zb);
  automation_id_13->add_actions({lambdaaction_id_14, zigbee_reportaction_id_8});
  button_4->set_component_source(LOG_STR("gpio.binary_sensor"));
  App.register_component(button_4);
  esp32_esp32internalgpiopin_id_10 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_10->set_pin(::GPIO_NUM_5);
  esp32_esp32internalgpiopin_id_10->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_10->set_flags((gpio::Flags::FLAG_INPUT | gpio::Flags::FLAG_PULLDOWN));
  button_4->set_pin(esp32_esp32internalgpiopin_id_10);
  button_4->set_use_interrupt(false);
  // sensor.max17048:
  //   platform: max17048
  //   id: max_17048
  //   battery_voltage:
  //     name: Battery Voltage
  //     id: batt_v
  //     disabled_by_default: false
  //     force_update: false
  //     unit_of_measurement: V
  //     accuracy_decimals: 3
  //     state_class: measurement
  //   battery_level:
  //     name: Battery Level
  //     id: batt_pct
  //     filters:
  //       - lambda: !lambda |
  //           if (x > 100) return 100;
  //           else return (x);
  //         type_id: sensor_lambdafilter_id
  //     disabled_by_default: false
  //     force_update: false
  //     unit_of_measurement: '%'
  //     accuracy_decimals: 2
  //     device_class: battery
  //     state_class: measurement
  //   update_interval: 60s
  //   i2c_id: i2c_bus
  //   address: 0x36
  max_17048 = new max17048::MAX17048Component();
  max_17048->set_update_interval(60000);
  max_17048->set_component_source(LOG_STR("max17048.sensor"));
  App.register_component(max_17048);
  max_17048->set_i2c_bus(i2c_bus);
  max_17048->set_i2c_address(0x36);
  batt_v = new sensor::Sensor();
  App.register_sensor(batt_v);
  batt_v->set_name("Battery Voltage");
  batt_v->set_object_id("battery_voltage");
  batt_v->set_state_class(sensor::STATE_CLASS_MEASUREMENT);
  batt_v->set_unit_of_measurement("V");
  batt_v->set_accuracy_decimals(3);
  max_17048->set_battery_v_sensor(batt_v);
  batt_pct = new sensor::Sensor();
  App.register_sensor(batt_pct);
  batt_pct->set_name("Battery Level");
  batt_pct->set_object_id("battery_level");
  batt_pct->set_device_class("battery");
  batt_pct->set_state_class(sensor::STATE_CLASS_MEASUREMENT);
  batt_pct->set_unit_of_measurement("%");
  batt_pct->set_accuracy_decimals(2);
  sensor_lambdafilter_id = new sensor::StatelessLambdaFilter([](float x) -> esphome::optional<float> {
      #line 322 "btn1_zigbee_simple_FIXED.yaml"
      if (x > 100) return 100;
      else return (x);
      
  });
  batt_pct->set_filters({sensor_lambdafilter_id});
  max_17048->set_battery_soc_sensor(batt_pct);
  // switch.template:
  //   platform: template
  //   name: Prevent Sleep
  //   id: prevent_sleep
  //   icon: mdi:sleep
  //   restore_mode: RESTORE_DEFAULT_ON
  //   optimistic: true
  //   entity_category: config
  //   on_turn_on:
  //     - then:
  //         - lambda: !lambda |-
  //             id(deep_sleep_1).prevent_deep_sleep();
  //           type_id: lambdaaction_id_15
  //       automation_id: automation_id_14
  //       trigger_id: switch__switchturnontrigger_id
  //   on_turn_off:
  //     - then:
  //         - lambda: !lambda |-
  //             id(deep_sleep_1).allow_deep_sleep();
  //           type_id: lambdaaction_id_16
  //       automation_id: automation_id_15
  //       trigger_id: switch__switchturnofftrigger_id
  //   disabled_by_default: false
  //   assumed_state: false
  prevent_sleep = new template_::TemplateSwitch();
  App.register_switch(prevent_sleep);
  prevent_sleep->set_name("Prevent Sleep");
  prevent_sleep->set_object_id("prevent_sleep");
  prevent_sleep->set_icon("mdi:sleep");
  prevent_sleep->set_entity_category(::ENTITY_CATEGORY_CONFIG);
  switch__switchturnontrigger_id = new switch_::SwitchTurnOnTrigger(prevent_sleep);
  automation_id_14 = new Automation<>(switch__switchturnontrigger_id);
  lambdaaction_id_15 = new StatelessLambdaAction<>([]() -> void {
      #line 340 "btn1_zigbee_simple_FIXED.yaml"
      deep_sleep_1->prevent_deep_sleep();
  });
  automation_id_14->add_actions({lambdaaction_id_15});
  switch__switchturnofftrigger_id = new switch_::SwitchTurnOffTrigger(prevent_sleep);
  automation_id_15 = new Automation<>(switch__switchturnofftrigger_id);
  lambdaaction_id_16 = new StatelessLambdaAction<>([]() -> void {
      #line 344 "btn1_zigbee_simple_FIXED.yaml"
      deep_sleep_1->allow_deep_sleep();
  });
  automation_id_15->add_actions({lambdaaction_id_16});
  prevent_sleep->set_restore_mode(switch_::SWITCH_RESTORE_DEFAULT_ON);
  prevent_sleep->set_component_source(LOG_STR("template.switch"));
  App.register_component(prevent_sleep);
  prevent_sleep->set_optimistic(true);
  prevent_sleep->set_assumed_state(false);
  // switch.factory_reset:
  //   platform: factory_reset
  //   id: factory_reset_switch
  //   disabled_by_default: true
  //   restore_mode: ALWAYS_OFF
  //   entity_category: config
  //   icon: mdi:restart-alert
  //   name: factory_reset_switch
  //   internal: true
  factory_reset_switch = new factory_reset::FactoryResetSwitch();
  App.register_switch(factory_reset_switch);
  factory_reset_switch->set_name("factory_reset_switch");
  factory_reset_switch->set_object_id("factory_reset_switch");
  factory_reset_switch->set_disabled_by_default(true);
  factory_reset_switch->set_internal(true);
  factory_reset_switch->set_icon("mdi:restart-alert");
  factory_reset_switch->set_entity_category(::ENTITY_CATEGORY_CONFIG);
  factory_reset_switch->set_restore_mode(switch_::SWITCH_ALWAYS_OFF);
  factory_reset_switch->set_component_source(LOG_STR("factory_reset.switch"));
  App.register_component(factory_reset_switch);
  // switch.gpio:
  //   platform: gpio
  //   pin:
  //     number: 19
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_11
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   name: Power Latch
  //   id: power_latch
  //   restore_mode: ALWAYS_ON
  //   setup_priority: 2000.0
  //   internal: true
  //   disabled_by_default: false
  //   interlock_wait_time: 0ms
  power_latch = new gpio::GPIOSwitch();
  App.register_switch(power_latch);
  power_latch->set_name("Power Latch");
  power_latch->set_object_id("power_latch");
  power_latch->set_internal(true);
  power_latch->set_restore_mode(switch_::SWITCH_ALWAYS_ON);
  power_latch->set_setup_priority(2000.0f);
  power_latch->set_component_source(LOG_STR("gpio.switch"));
  App.register_component(power_latch);
  esp32_esp32internalgpiopin_id_11 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_11->set_pin(::GPIO_NUM_19);
  esp32_esp32internalgpiopin_id_11->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_11->set_flags(gpio::Flags::FLAG_OUTPUT);
  power_latch->set_pin(esp32_esp32internalgpiopin_id_11);
  // switch.gpio:
  //   platform: gpio
  //   pin:
  //     number: 18
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_12
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   name: Accessory Power
  //   id: accessory_power
  //   restore_mode: ALWAYS_ON
  //   setup_priority: 2000.0
  //   internal: true
  //   disabled_by_default: false
  //   interlock_wait_time: 0ms
  accessory_power = new gpio::GPIOSwitch();
  App.register_switch(accessory_power);
  accessory_power->set_name("Accessory Power");
  accessory_power->set_object_id("accessory_power");
  accessory_power->set_internal(true);
  accessory_power->set_restore_mode(switch_::SWITCH_ALWAYS_ON);
  accessory_power->set_setup_priority(2000.0f);
  accessory_power->set_component_source(LOG_STR("gpio.switch"));
  App.register_component(accessory_power);
  esp32_esp32internalgpiopin_id_12 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_12->set_pin(::GPIO_NUM_18);
  esp32_esp32internalgpiopin_id_12->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_12->set_flags(gpio::Flags::FLAG_OUTPUT);
  accessory_power->set_pin(esp32_esp32internalgpiopin_id_12);
  // light.esp32_rmt_led_strip:
  //   platform: esp32_rmt_led_strip
  //   id: rgb_light
  //   name: RGB Light
  //   pin: 21
  //   default_transition_length: 0s
  //   chipset: WS2812
  //   num_leds: 4
  //   rgb_order: GRB
  //   internal: true
  //   effects:
  //     - pulse:
  //         name: Slow Pulse
  //         transition_length: 1000ms
  //         update_interval: 1000ms
  //         min_brightness: 0.0
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id
  //     - pulse:
  //         name: Fast Blink
  //         transition_length: 100ms
  //         update_interval: 100ms
  //         min_brightness: 0.5
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_2
  //   disabled_by_default: false
  //   restore_mode: ALWAYS_OFF
  //   gamma_correct: 2.8
  //   flash_transition_length: 0s
  //   output_id: esp32_rmt_led_strip_esp32rmtledstriplightoutput_id
  //   rmt_symbols: 96
  //   is_rgbw: false
  //   is_wrgb: false
  //   use_psram: true
  //   reset_high: 0us
  //   reset_low: 0us
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id = new esp32_rmt_led_strip::ESP32RMTLEDStripLightOutput();
  rgb_light = new light::AddressableLightState(esp32_rmt_led_strip_esp32rmtledstriplightoutput_id);
  App.register_light(rgb_light);
  rgb_light->set_component_source(LOG_STR("light"));
  App.register_component(rgb_light);
  rgb_light->set_name("RGB Light");
  rgb_light->set_object_id("rgb_light");
  rgb_light->set_internal(true);
  rgb_light->set_restore_mode(light::LIGHT_ALWAYS_OFF);
  rgb_light->set_default_transition_length(0);
  rgb_light->set_flash_transition_length(0);
  rgb_light->set_gamma_correct(2.8f);
  light_pulselighteffect_id = new light::PulseLightEffect("Slow Pulse");
  light_pulselighteffect_id->set_transition_on_length(1000);
  light_pulselighteffect_id->set_transition_off_length(1000);
  light_pulselighteffect_id->set_update_interval(1000);
  light_pulselighteffect_id->set_min_max_brightness(0.0f, 1.0f);
  light_pulselighteffect_id_2 = new light::PulseLightEffect("Fast Blink");
  light_pulselighteffect_id_2->set_transition_on_length(100);
  light_pulselighteffect_id_2->set_transition_off_length(100);
  light_pulselighteffect_id_2->set_update_interval(100);
  light_pulselighteffect_id_2->set_min_max_brightness(0.5f, 1.0f);
  rgb_light->add_effects({light_pulselighteffect_id, light_pulselighteffect_id_2});
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_component_source(LOG_STR("esp32_rmt_led_strip.light"));
  App.register_component(esp32_rmt_led_strip_esp32rmtledstriplightoutput_id);
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_num_leds(4);
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_pin(21);
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_led_params(400, 1000, 1000, 400, 0, 0);
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_rgb_order(esp32_rmt_led_strip::ORDER_GRB);
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_is_rgbw(false);
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_is_wrgb(false);
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_use_psram(true);
  esp32_rmt_led_strip_esp32rmtledstriplightoutput_id->set_rmt_symbols(96);
  // light.partition:
  //   platform: partition
  //   name: BTN 1 Light
  //   id: btn_1_light
  //   default_transition_length: 0s
  //   segments:
  //     - id: rgb_light
  //       from: 0
  //       to: 0
  //       reversed: false
  //   effects:
  //     - pulse:
  //         name: Slow Pulse
  //         transition_length: 1000ms
  //         update_interval: 1000ms
  //         min_brightness: 0.0
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_3
  //     - pulse:
  //         name: Fast Blink
  //         transition_length: 100ms
  //         update_interval: 100ms
  //         min_brightness: 0.5
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_4
  //   disabled_by_default: false
  //   restore_mode: ALWAYS_OFF
  //   gamma_correct: 2.8
  //   flash_transition_length: 0s
  //   output_id: partition_partitionlightoutput_id
  partition_partitionlightoutput_id = new partition::PartitionLightOutput({partition::AddressableSegment(rgb_light, 0, 1, false)});
  partition_partitionlightoutput_id->set_component_source(LOG_STR("partition.light"));
  App.register_component(partition_partitionlightoutput_id);
  btn_1_light = new light::AddressableLightState(partition_partitionlightoutput_id);
  App.register_light(btn_1_light);
  btn_1_light->set_component_source(LOG_STR("light"));
  App.register_component(btn_1_light);
  btn_1_light->set_name("BTN 1 Light");
  btn_1_light->set_object_id("btn_1_light");
  btn_1_light->set_restore_mode(light::LIGHT_ALWAYS_OFF);
  btn_1_light->set_default_transition_length(0);
  btn_1_light->set_flash_transition_length(0);
  btn_1_light->set_gamma_correct(2.8f);
  light_pulselighteffect_id_3 = new light::PulseLightEffect("Slow Pulse");
  light_pulselighteffect_id_3->set_transition_on_length(1000);
  light_pulselighteffect_id_3->set_transition_off_length(1000);
  light_pulselighteffect_id_3->set_update_interval(1000);
  light_pulselighteffect_id_3->set_min_max_brightness(0.0f, 1.0f);
  light_pulselighteffect_id_4 = new light::PulseLightEffect("Fast Blink");
  light_pulselighteffect_id_4->set_transition_on_length(100);
  light_pulselighteffect_id_4->set_transition_off_length(100);
  light_pulselighteffect_id_4->set_update_interval(100);
  light_pulselighteffect_id_4->set_min_max_brightness(0.5f, 1.0f);
  btn_1_light->add_effects({light_pulselighteffect_id_3, light_pulselighteffect_id_4});
  // light.partition:
  //   platform: partition
  //   name: BTN 2 Light
  //   id: btn_2_light
  //   default_transition_length: 0s
  //   segments:
  //     - id: rgb_light
  //       from: 1
  //       to: 1
  //       reversed: false
  //   effects:
  //     - pulse:
  //         name: Slow Pulse
  //         transition_length: 1000ms
  //         update_interval: 1000ms
  //         min_brightness: 0.0
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_5
  //     - pulse:
  //         name: Fast Blink
  //         transition_length: 100ms
  //         update_interval: 100ms
  //         min_brightness: 0.5
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_6
  //   disabled_by_default: false
  //   restore_mode: ALWAYS_OFF
  //   gamma_correct: 2.8
  //   flash_transition_length: 0s
  //   output_id: partition_partitionlightoutput_id_2
  partition_partitionlightoutput_id_2 = new partition::PartitionLightOutput({partition::AddressableSegment(rgb_light, 1, 1, false)});
  partition_partitionlightoutput_id_2->set_component_source(LOG_STR("partition.light"));
  App.register_component(partition_partitionlightoutput_id_2);
  btn_2_light = new light::AddressableLightState(partition_partitionlightoutput_id_2);
  App.register_light(btn_2_light);
  btn_2_light->set_component_source(LOG_STR("light"));
  App.register_component(btn_2_light);
  btn_2_light->set_name("BTN 2 Light");
  btn_2_light->set_object_id("btn_2_light");
  btn_2_light->set_restore_mode(light::LIGHT_ALWAYS_OFF);
  btn_2_light->set_default_transition_length(0);
  btn_2_light->set_flash_transition_length(0);
  btn_2_light->set_gamma_correct(2.8f);
  light_pulselighteffect_id_5 = new light::PulseLightEffect("Slow Pulse");
  light_pulselighteffect_id_5->set_transition_on_length(1000);
  light_pulselighteffect_id_5->set_transition_off_length(1000);
  light_pulselighteffect_id_5->set_update_interval(1000);
  light_pulselighteffect_id_5->set_min_max_brightness(0.0f, 1.0f);
  light_pulselighteffect_id_6 = new light::PulseLightEffect("Fast Blink");
  light_pulselighteffect_id_6->set_transition_on_length(100);
  light_pulselighteffect_id_6->set_transition_off_length(100);
  light_pulselighteffect_id_6->set_update_interval(100);
  light_pulselighteffect_id_6->set_min_max_brightness(0.5f, 1.0f);
  btn_2_light->add_effects({light_pulselighteffect_id_5, light_pulselighteffect_id_6});
  // light.partition:
  //   platform: partition
  //   name: BTN 3 Light
  //   id: btn_3_light
  //   default_transition_length: 0s
  //   segments:
  //     - id: rgb_light
  //       from: 2
  //       to: 2
  //       reversed: false
  //   effects:
  //     - pulse:
  //         name: Slow Pulse
  //         transition_length: 1000ms
  //         update_interval: 1000ms
  //         min_brightness: 0.0
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_7
  //     - pulse:
  //         name: Fast Blink
  //         transition_length: 100ms
  //         update_interval: 100ms
  //         min_brightness: 0.5
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_8
  //   disabled_by_default: false
  //   restore_mode: ALWAYS_OFF
  //   gamma_correct: 2.8
  //   flash_transition_length: 0s
  //   output_id: partition_partitionlightoutput_id_3
  partition_partitionlightoutput_id_3 = new partition::PartitionLightOutput({partition::AddressableSegment(rgb_light, 2, 1, false)});
  partition_partitionlightoutput_id_3->set_component_source(LOG_STR("partition.light"));
  App.register_component(partition_partitionlightoutput_id_3);
  btn_3_light = new light::AddressableLightState(partition_partitionlightoutput_id_3);
  App.register_light(btn_3_light);
  btn_3_light->set_component_source(LOG_STR("light"));
  App.register_component(btn_3_light);
  btn_3_light->set_name("BTN 3 Light");
  btn_3_light->set_object_id("btn_3_light");
  btn_3_light->set_restore_mode(light::LIGHT_ALWAYS_OFF);
  btn_3_light->set_default_transition_length(0);
  btn_3_light->set_flash_transition_length(0);
  btn_3_light->set_gamma_correct(2.8f);
  light_pulselighteffect_id_7 = new light::PulseLightEffect("Slow Pulse");
  light_pulselighteffect_id_7->set_transition_on_length(1000);
  light_pulselighteffect_id_7->set_transition_off_length(1000);
  light_pulselighteffect_id_7->set_update_interval(1000);
  light_pulselighteffect_id_7->set_min_max_brightness(0.0f, 1.0f);
  light_pulselighteffect_id_8 = new light::PulseLightEffect("Fast Blink");
  light_pulselighteffect_id_8->set_transition_on_length(100);
  light_pulselighteffect_id_8->set_transition_off_length(100);
  light_pulselighteffect_id_8->set_update_interval(100);
  light_pulselighteffect_id_8->set_min_max_brightness(0.5f, 1.0f);
  btn_3_light->add_effects({light_pulselighteffect_id_7, light_pulselighteffect_id_8});
  // light.partition:
  //   platform: partition
  //   name: BTN 4 Light
  //   id: btn_4_light
  //   default_transition_length: 0s
  //   segments:
  //     - id: rgb_light
  //       from: 3
  //       to: 3
  //       reversed: false
  //   effects:
  //     - pulse:
  //         name: Slow Pulse
  //         transition_length: 1000ms
  //         update_interval: 1000ms
  //         min_brightness: 0.0
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_9
  //     - pulse:
  //         name: Fast Blink
  //         transition_length: 100ms
  //         update_interval: 100ms
  //         min_brightness: 0.5
  //         max_brightness: 1.0
  //       type_id: light_pulselighteffect_id_10
  //   disabled_by_default: false
  //   restore_mode: ALWAYS_OFF
  //   gamma_correct: 2.8
  //   flash_transition_length: 0s
  //   output_id: partition_partitionlightoutput_id_4
  partition_partitionlightoutput_id_4 = new partition::PartitionLightOutput({partition::AddressableSegment(rgb_light, 3, 1, false)});
  partition_partitionlightoutput_id_4->set_component_source(LOG_STR("partition.light"));
  App.register_component(partition_partitionlightoutput_id_4);
  btn_4_light = new light::AddressableLightState(partition_partitionlightoutput_id_4);
  App.register_light(btn_4_light);
  btn_4_light->set_component_source(LOG_STR("light"));
  App.register_component(btn_4_light);
  btn_4_light->set_name("BTN 4 Light");
  btn_4_light->set_object_id("btn_4_light");
  btn_4_light->set_restore_mode(light::LIGHT_ALWAYS_OFF);
  btn_4_light->set_default_transition_length(0);
  btn_4_light->set_flash_transition_length(0);
  btn_4_light->set_gamma_correct(2.8f);
  light_pulselighteffect_id_9 = new light::PulseLightEffect("Slow Pulse");
  light_pulselighteffect_id_9->set_transition_on_length(1000);
  light_pulselighteffect_id_9->set_transition_off_length(1000);
  light_pulselighteffect_id_9->set_update_interval(1000);
  light_pulselighteffect_id_9->set_min_max_brightness(0.0f, 1.0f);
  light_pulselighteffect_id_10 = new light::PulseLightEffect("Fast Blink");
  light_pulselighteffect_id_10->set_transition_on_length(100);
  light_pulselighteffect_id_10->set_transition_off_length(100);
  light_pulselighteffect_id_10->set_update_interval(100);
  light_pulselighteffect_id_10->set_min_max_brightness(0.5f, 1.0f);
  btn_4_light->add_effects({light_pulselighteffect_id_9, light_pulselighteffect_id_10});
  // button.restart:
  //   platform: restart
  //   icon: mdi:power-cycle
  //   name: ESP Reboot
  //   disabled_by_default: false
  //   id: restart_restartbutton_id
  //   entity_category: config
  //   device_class: restart
  restart_restartbutton_id = new restart::RestartButton();
  restart_restartbutton_id->set_component_source(LOG_STR("restart.button"));
  App.register_component(restart_restartbutton_id);
  App.register_button(restart_restartbutton_id);
  restart_restartbutton_id->set_name("ESP Reboot");
  restart_restartbutton_id->set_object_id("esp_reboot");
  restart_restartbutton_id->set_icon("mdi:power-cycle");
  restart_restartbutton_id->set_entity_category(::ENTITY_CATEGORY_CONFIG);
  restart_restartbutton_id->set_device_class("restart");
  // preferences:
  //   id: preferences_intervalsyncer_id
  //   flash_write_interval: 60s
  preferences_intervalsyncer_id = new preferences::IntervalSyncer();
  preferences_intervalsyncer_id->set_write_interval(60000);
  preferences_intervalsyncer_id->set_component_source(LOG_STR("preferences"));
  App.register_component(preferences_intervalsyncer_id);
  zigbee_zigbeeattribute_id->connect<bool>(btn_1_light);
  zb->add_cluster(1, ::ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_2 = new zigbee::ZigBeeAttribute(zb, 1, ::ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_U8, 1);
  zigbee_zigbeeattribute_id_2->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_2);
  zigbee_zigbeeattribute_id_2->add_attr(0, 0, 255);
  zigbee_zigbeeattribute_id_2->set_report(false);
  zigbee_zigbeeattribute_id_2->connect<uint8_t>(btn_1_light);
  zb->add_cluster(1, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_3 = new zigbee::ZigBeeAttribute(zb, 1, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 3, ::ESP_ZB_ZCL_ATTR_TYPE_U16, 1);
  zigbee_zigbeeattribute_id_3->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_3);
  zigbee_zigbeeattribute_id_3->add_attr(0, 0, 24939);
  zigbee_zigbeeattribute_id_3->set_report(false);
  zigbee_zigbeeattribute_id_3->connect<uint16_t>(btn_1_light);
  zigbee_zigbeeattribute_id_4 = new zigbee::ZigBeeAttribute(zb, 1, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 4, ::ESP_ZB_ZCL_ATTR_TYPE_U16, 1);
  zigbee_zigbeeattribute_id_4->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_4);
  zigbee_zigbeeattribute_id_4->add_attr(0, 0, 24701);
  zigbee_zigbeeattribute_id_4->set_report(false);
  zigbee_zigbeeattribute_id_4->connect<uint16_t>(btn_1_light);
  zigbee_zigbeeattribute_id_5 = new zigbee::ZigBeeAttribute(zb, 1, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 16394, ::ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 1);
  zigbee_zigbeeattribute_id_5->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_5);
  zigbee_zigbeeattribute_id_5->add_attr(0, 0, 8);
  zb->create_default_cluster(2, ::ESP_ZB_HA_COLOR_DIMMABLE_LIGHT_DEVICE_ID);
  zb->add_cluster(2, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(2, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_6 = new zigbee::ZigBeeAttribute(zb, 2, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_6->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_6);
  zigbee_zigbeeattribute_id_6->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_6->set_report(false);
  zigbee_zigbeeattribute_id_6->connect<bool>(btn_2_light);
  zb->add_cluster(2, ::ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_7 = new zigbee::ZigBeeAttribute(zb, 2, ::ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_U8, 1);
  zigbee_zigbeeattribute_id_7->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_7);
  zigbee_zigbeeattribute_id_7->add_attr(0, 0, 255);
  zigbee_zigbeeattribute_id_7->set_report(false);
  zigbee_zigbeeattribute_id_7->connect<uint8_t>(btn_2_light);
  zb->add_cluster(2, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_8 = new zigbee::ZigBeeAttribute(zb, 2, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 3, ::ESP_ZB_ZCL_ATTR_TYPE_U16, 1);
  zigbee_zigbeeattribute_id_8->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_8);
  zigbee_zigbeeattribute_id_8->add_attr(0, 0, 24939);
  zigbee_zigbeeattribute_id_8->set_report(false);
  zigbee_zigbeeattribute_id_8->connect<uint16_t>(btn_2_light);
  zigbee_zigbeeattribute_id_9 = new zigbee::ZigBeeAttribute(zb, 2, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 4, ::ESP_ZB_ZCL_ATTR_TYPE_U16, 1);
  zigbee_zigbeeattribute_id_9->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_9);
  zigbee_zigbeeattribute_id_9->add_attr(0, 0, 24701);
  zigbee_zigbeeattribute_id_9->set_report(false);
  zigbee_zigbeeattribute_id_9->connect<uint16_t>(btn_2_light);
  zigbee_zigbeeattribute_id_10 = new zigbee::ZigBeeAttribute(zb, 2, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 16394, ::ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 1);
  zigbee_zigbeeattribute_id_10->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_10);
  zigbee_zigbeeattribute_id_10->add_attr(0, 0, 8);
  zb->create_default_cluster(3, ::ESP_ZB_HA_COLOR_DIMMABLE_LIGHT_DEVICE_ID);
  zb->add_cluster(3, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(3, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_11 = new zigbee::ZigBeeAttribute(zb, 3, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_11->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_11);
  zigbee_zigbeeattribute_id_11->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_11->set_report(false);
  zigbee_zigbeeattribute_id_11->connect<bool>(btn_3_light);
  zb->add_cluster(3, ::ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_12 = new zigbee::ZigBeeAttribute(zb, 3, ::ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_U8, 1);
  zigbee_zigbeeattribute_id_12->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_12);
  zigbee_zigbeeattribute_id_12->add_attr(0, 0, 255);
  zigbee_zigbeeattribute_id_12->set_report(false);
  zigbee_zigbeeattribute_id_12->connect<uint8_t>(btn_3_light);
  zb->add_cluster(3, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_13 = new zigbee::ZigBeeAttribute(zb, 3, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 3, ::ESP_ZB_ZCL_ATTR_TYPE_U16, 1);
  zigbee_zigbeeattribute_id_13->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_13);
  zigbee_zigbeeattribute_id_13->add_attr(0, 0, 24939);
  zigbee_zigbeeattribute_id_13->set_report(false);
  zigbee_zigbeeattribute_id_13->connect<uint16_t>(btn_3_light);
  zigbee_zigbeeattribute_id_14 = new zigbee::ZigBeeAttribute(zb, 3, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 4, ::ESP_ZB_ZCL_ATTR_TYPE_U16, 1);
  zigbee_zigbeeattribute_id_14->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_14);
  zigbee_zigbeeattribute_id_14->add_attr(0, 0, 24701);
  zigbee_zigbeeattribute_id_14->set_report(false);
  zigbee_zigbeeattribute_id_14->connect<uint16_t>(btn_3_light);
  zigbee_zigbeeattribute_id_15 = new zigbee::ZigBeeAttribute(zb, 3, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 16394, ::ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 1);
  zigbee_zigbeeattribute_id_15->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_15);
  zigbee_zigbeeattribute_id_15->add_attr(0, 0, 8);
  zb->create_default_cluster(4, ::ESP_ZB_HA_COLOR_DIMMABLE_LIGHT_DEVICE_ID);
  zb->add_cluster(4, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(4, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_16 = new zigbee::ZigBeeAttribute(zb, 4, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_16->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_16);
  zigbee_zigbeeattribute_id_16->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_16->set_report(false);
  zigbee_zigbeeattribute_id_16->connect<bool>(btn_4_light);
  zb->add_cluster(4, ::ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_17 = new zigbee::ZigBeeAttribute(zb, 4, ::ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_U8, 1);
  zigbee_zigbeeattribute_id_17->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_17);
  zigbee_zigbeeattribute_id_17->add_attr(0, 0, 255);
  zigbee_zigbeeattribute_id_17->set_report(false);
  zigbee_zigbeeattribute_id_17->connect<uint8_t>(btn_4_light);
  zb->add_cluster(4, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_18 = new zigbee::ZigBeeAttribute(zb, 4, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 3, ::ESP_ZB_ZCL_ATTR_TYPE_U16, 1);
  zigbee_zigbeeattribute_id_18->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_18);
  zigbee_zigbeeattribute_id_18->add_attr(0, 0, 24939);
  zigbee_zigbeeattribute_id_18->set_report(false);
  zigbee_zigbeeattribute_id_18->connect<uint16_t>(btn_4_light);
  zigbee_zigbeeattribute_id_19 = new zigbee::ZigBeeAttribute(zb, 4, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 4, ::ESP_ZB_ZCL_ATTR_TYPE_U16, 1);
  zigbee_zigbeeattribute_id_19->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_19);
  zigbee_zigbeeattribute_id_19->add_attr(0, 0, 24701);
  zigbee_zigbeeattribute_id_19->set_report(false);
  zigbee_zigbeeattribute_id_19->connect<uint16_t>(btn_4_light);
  zigbee_zigbeeattribute_id_20 = new zigbee::ZigBeeAttribute(zb, 4, ::ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 16394, ::ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 1);
  zigbee_zigbeeattribute_id_20->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_20);
  zigbee_zigbeeattribute_id_20->add_attr(0, 0, 8);
  zb->create_default_cluster(5, ::ESP_ZB_HA_ON_OFF_OUTPUT_DEVICE_ID);
  zb->add_cluster(5, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(5, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_21 = new zigbee::ZigBeeAttribute(zb, 5, ::ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 0, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_21->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_21);
  zigbee_zigbeeattribute_id_21->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_21->set_report(false);
  zigbee_zigbeeattribute_id_21->connect<bool>(prevent_sleep);
  zb->create_default_cluster(6, ::ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID);
  zb->add_cluster(6, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(6, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_22 = new zigbee::ZigBeeAttribute(zb, 6, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 85, ::ESP_ZB_ZCL_ATTR_TYPE_SINGLE, 1);
  zigbee_zigbeeattribute_id_22->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_22);
  zigbee_zigbeeattribute_id_22->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_22->set_report(false);
  zigbee_zigbeeattribute_id_22->connect<float>(batt_v);
  zigbee_zigbeeattribute_id_23 = new zigbee::ZigBeeAttribute(zb, 6, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 81, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_23->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_23);
  zigbee_zigbeeattribute_id_23->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_24 = new zigbee::ZigBeeAttribute(zb, 6, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 111, ::ESP_ZB_ZCL_ATTR_TYPE_8BITMAP, 1);
  zigbee_zigbeeattribute_id_24->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_24);
  zigbee_zigbeeattribute_id_24->add_attr(0, 0, 0);
  zigbee_zigbeeattribute_id_25 = new zigbee::ZigBeeAttribute(zb, 6, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 28, ::ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 1);
  zigbee_zigbeeattribute_id_25->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_25);
  zigbee_zigbeeattribute_id_25->add_attr(0, 15, "Battery Voltage");
  zigbee_zigbeeattribute_id_26 = new zigbee::ZigBeeAttribute(zb, 6, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 117, ::ESP_ZB_ZCL_ATTR_TYPE_16BIT_ENUM, 1);
  zigbee_zigbeeattribute_id_26->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_26);
  zigbee_zigbeeattribute_id_26->add_attr(0, 0, 5);
  zb->create_default_cluster(7, ::ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID);
  zb->add_cluster(7, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(7, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_27 = new zigbee::ZigBeeAttribute(zb, 7, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 85, ::ESP_ZB_ZCL_ATTR_TYPE_SINGLE, 1);
  zigbee_zigbeeattribute_id_27->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_27);
  zigbee_zigbeeattribute_id_27->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_27->set_report(false);
  zigbee_zigbeeattribute_id_27->connect<float>(batt_pct);
  zigbee_zigbeeattribute_id_28 = new zigbee::ZigBeeAttribute(zb, 7, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 81, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_28->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_28);
  zigbee_zigbeeattribute_id_28->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_29 = new zigbee::ZigBeeAttribute(zb, 7, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 111, ::ESP_ZB_ZCL_ATTR_TYPE_8BITMAP, 1);
  zigbee_zigbeeattribute_id_29->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_29);
  zigbee_zigbeeattribute_id_29->add_attr(0, 0, 0);
  zigbee_zigbeeattribute_id_30 = new zigbee::ZigBeeAttribute(zb, 7, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 28, ::ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 1);
  zigbee_zigbeeattribute_id_30->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_30);
  zigbee_zigbeeattribute_id_30->add_attr(0, 13, "Battery Level");
  zigbee_zigbeeattribute_id_31 = new zigbee::ZigBeeAttribute(zb, 7, ::ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 117, ::ESP_ZB_ZCL_ATTR_TYPE_16BIT_ENUM, 1);
  zigbee_zigbeeattribute_id_31->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_31);
  zigbee_zigbeeattribute_id_31->add_attr(0, 0, 98);
  zb->create_default_cluster(8, ::ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID);
  zb->add_cluster(8, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(8, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_32 = new zigbee::ZigBeeAttribute(zb, 8, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 85, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_32->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_32);
  zigbee_zigbeeattribute_id_32->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_32->set_report(false);
  zigbee_zigbeeattribute_id_32->connect<bool>(button_1);
  zigbee_zigbeeattribute_id_33 = new zigbee::ZigBeeAttribute(zb, 8, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 81, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_33->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_33);
  zigbee_zigbeeattribute_id_33->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_34 = new zigbee::ZigBeeAttribute(zb, 8, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 111, ::ESP_ZB_ZCL_ATTR_TYPE_8BITMAP, 1);
  zigbee_zigbeeattribute_id_34->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_34);
  zigbee_zigbeeattribute_id_34->add_attr(0, 0, 0);
  zigbee_zigbeeattribute_id_35 = new zigbee::ZigBeeAttribute(zb, 8, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 28, ::ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 1);
  zigbee_zigbeeattribute_id_35->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_35);
  zigbee_zigbeeattribute_id_35->add_attr(0, 8, "Button 1");
  zb->create_default_cluster(9, ::ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID);
  zb->add_cluster(9, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(9, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_36 = new zigbee::ZigBeeAttribute(zb, 9, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 85, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_36->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_36);
  zigbee_zigbeeattribute_id_36->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_36->set_report(false);
  zigbee_zigbeeattribute_id_36->connect<bool>(button_2);
  zigbee_zigbeeattribute_id_37 = new zigbee::ZigBeeAttribute(zb, 9, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 81, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_37->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_37);
  zigbee_zigbeeattribute_id_37->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_38 = new zigbee::ZigBeeAttribute(zb, 9, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 111, ::ESP_ZB_ZCL_ATTR_TYPE_8BITMAP, 1);
  zigbee_zigbeeattribute_id_38->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_38);
  zigbee_zigbeeattribute_id_38->add_attr(0, 0, 0);
  zigbee_zigbeeattribute_id_39 = new zigbee::ZigBeeAttribute(zb, 9, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 28, ::ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 1);
  zigbee_zigbeeattribute_id_39->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_39);
  zigbee_zigbeeattribute_id_39->add_attr(0, 8, "Button 2");
  zb->create_default_cluster(10, ::ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID);
  zb->add_cluster(10, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(10, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_40 = new zigbee::ZigBeeAttribute(zb, 10, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 85, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_40->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_40);
  zigbee_zigbeeattribute_id_40->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_40->set_report(false);
  zigbee_zigbeeattribute_id_40->connect<bool>(button_3);
  zigbee_zigbeeattribute_id_41 = new zigbee::ZigBeeAttribute(zb, 10, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 81, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_41->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_41);
  zigbee_zigbeeattribute_id_41->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_42 = new zigbee::ZigBeeAttribute(zb, 10, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 111, ::ESP_ZB_ZCL_ATTR_TYPE_8BITMAP, 1);
  zigbee_zigbeeattribute_id_42->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_42);
  zigbee_zigbeeattribute_id_42->add_attr(0, 0, 0);
  zigbee_zigbeeattribute_id_43 = new zigbee::ZigBeeAttribute(zb, 10, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 28, ::ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 1);
  zigbee_zigbeeattribute_id_43->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_43);
  zigbee_zigbeeattribute_id_43->add_attr(0, 8, "Button 3");
  zb->create_default_cluster(11, ::ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID);
  zb->add_cluster(11, ::ESP_ZB_ZCL_CLUSTER_ID_BASIC, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zb->add_cluster(11, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  zigbee_zigbeeattribute_id_44 = new zigbee::ZigBeeAttribute(zb, 11, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 85, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_44->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_44);
  zigbee_zigbeeattribute_id_44->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_44->set_report(false);
  zigbee_zigbeeattribute_id_44->connect<bool>(button_4);
  zigbee_zigbeeattribute_id_45 = new zigbee::ZigBeeAttribute(zb, 11, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 81, ::ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1);
  zigbee_zigbeeattribute_id_45->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_45);
  zigbee_zigbeeattribute_id_45->add_attr(0, 0, false);
  zigbee_zigbeeattribute_id_46 = new zigbee::ZigBeeAttribute(zb, 11, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 111, ::ESP_ZB_ZCL_ATTR_TYPE_8BITMAP, 1);
  zigbee_zigbeeattribute_id_46->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_46);
  zigbee_zigbeeattribute_id_46->add_attr(0, 0, 0);
  zigbee_zigbeeattribute_id_47 = new zigbee::ZigBeeAttribute(zb, 11, ::ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, ::ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 28, ::ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 1);
  zigbee_zigbeeattribute_id_47->set_component_source(LOG_STR("zigbee"));
  App.register_component(zigbee_zigbeeattribute_id_47);
  zigbee_zigbeeattribute_id_47->add_attr(0, 8, "Button 4");
  zigbee_zigbeejointrigger_id = new zigbee::ZigBeeJoinTrigger(zb);
  automation_id = new Automation<>(zigbee_zigbeejointrigger_id);
  lambdaaction_id = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "*** ZIGBEE NETWORK JOINED ***");
  });
  automation_id->add_actions({lambdaaction_id});
  // globals:
  //   id: cycleCounter
  //   type: int
  //   restore_value: false
  //   initial_value: '0'
  cycleCounter = new globals::GlobalsComponent<int>(0);
  cycleCounter->set_component_source(LOG_STR("globals"));
  App.register_component(cycleCounter);
  // globals:
  //   id: button_press_timestamp
  //   restore_value: false
  //   type: uint32_t
  //   initial_value: '0'
  button_press_timestamp = new globals::GlobalsComponent<uint32_t>(0);
  button_press_timestamp->set_component_source(LOG_STR("globals"));
  App.register_component(button_press_timestamp);
  lambdaaction_id_4 = new StatelessLambdaAction<>([]() -> void {
      #line 186 "btn1_zigbee_simple_FIXED.yaml"
      button_press_timestamp->value() = millis();
  });
  automation_id_3->add_actions({lambdaaction_id_3, lambdaaction_id_4});
  binary_sensor_releasetrigger_id = new binary_sensor::ReleaseTrigger(reset_button);
  automation_id_4 = new Automation<>(binary_sensor_releasetrigger_id);
  lambdaaction_id_5 = new StatelessLambdaAction<>([]() -> void {
      #line 190 "btn1_zigbee_simple_FIXED.yaml"
      uint32_t hold_time = millis() - button_press_timestamp->value();
      ESP_LOGI("button", "Reset button released after %d ms", hold_time);
      
      if (hold_time >= 10000) {
         
        ESP_LOGI("button", "Factory reset triggered!");
        prevent_sleep->turn_on();
        factory_reset_switch->turn_on();
      }
      else if (hold_time >= 3000) {
         
        ESP_LOGI("button", "Prevent sleep activated");
        prevent_sleep->turn_on();
        deep_sleep_1->prevent_deep_sleep();
      }
  });
  automation_id_4->add_actions({lambdaaction_id_5});
  binary_sensor_clicktrigger_id = new binary_sensor::ClickTrigger(reset_button, 5000, 10000);
  automation_id_5 = new Automation<>(binary_sensor_clicktrigger_id);
  lambdaaction_id_6 = new StatelessLambdaAction<>([]() -> void {
      ESP_LOGI("main", "*** ZIGBEE RESET - Entering pairing mode ***");
  });
  zigbee_resetzigbeeaction_id = new zigbee::ResetZigbeeAction<>();
  zigbee_resetzigbeeaction_id->set_parent(zb);
  automation_id_5->add_actions({lambdaaction_id_6, zigbee_resetzigbeeaction_id});
  reset_button->set_component_source(LOG_STR("gpio.binary_sensor"));
  App.register_component(reset_button);
  esp32_esp32internalgpiopin_id_6 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_6->set_pin(::GPIO_NUM_9);
  esp32_esp32internalgpiopin_id_6->set_inverted(true);
  esp32_esp32internalgpiopin_id_6->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_6->set_flags((gpio::Flags::FLAG_INPUT | gpio::Flags::FLAG_PULLUP));
  reset_button->set_pin(esp32_esp32internalgpiopin_id_6);
  reset_button->set_interrupt_type(gpio::INTERRUPT_ANY_EDGE);
  // =========== AUTO GENERATED CODE END ============
  App.setup();
}

void loop() {
  App.loop();
}
