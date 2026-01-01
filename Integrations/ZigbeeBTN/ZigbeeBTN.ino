#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "ZigbeeCore.h"
#include "ep/ZigbeeLight.h"
#include "ep/ZigbeeSwitch.h"
#include <Preferences.h>

Preferences prefs;

#define LED_PIN               21
#define BUTTON_PIN            9  // ESP32-C6/H2 Boot button
#define ZIGBEE_LIGHT_ENDPOINT 10
#define ZIGBEE_SWITCH_ENDPOINT 5
#define ACCESSORY_POWER_PIN   18

// WS2812 LED color when ON (white at moderate brightness)
#define LED_BRIGHTNESS        64
#define LED_R                 LED_BRIGHTNESS
#define LED_G                 LED_BRIGHTNESS
#define LED_B                 LED_BRIGHTNESS

// Macro to calculate array size
#define PAIR_SIZE(TYPE_STR_PAIR) (sizeof(TYPE_STR_PAIR) / sizeof(TYPE_STR_PAIR[0]))

// Switch function types
typedef enum {
  SWITCH_ON_CONTROL,
  SWITCH_OFF_CONTROL,
  SWITCH_ONOFF_TOGGLE_CONTROL,
  SWITCH_LEVEL_UP_CONTROL,
  SWITCH_LEVEL_DOWN_CONTROL,
  SWITCH_LEVEL_CYCLE_CONTROL,
  SWITCH_COLOR_CONTROL,
} SwitchFunction;

typedef struct {
  uint8_t pin;
  SwitchFunction func;
} SwitchData;

typedef enum {
  SWITCH_IDLE,
  SWITCH_PRESS_ARMED,
  SWITCH_PRESS_DETECTED,
  SWITCH_PRESSED,
  SWITCH_RELEASE_DETECTED,
} SwitchState;

// Button configuration - maps button pin to function
static SwitchData buttonFunctionPair[] = {{BUTTON_PIN, SWITCH_ONOFF_TOGGLE_CONTROL}};

// Zigbee endpoints
ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);
ZigbeeSwitch zbSwitch = ZigbeeSwitch(ZIGBEE_SWITCH_ENDPOINT);

/********************* RGB LED functions **************************/
void setLED(bool value) {
  Serial.printf("setLED callback called with value: %s\n", value ? "ON" : "OFF");
  if (value) {
    neopixelWrite(LED_PIN, LED_R, LED_G, LED_B);
  } else {
    neopixelWrite(LED_PIN, 0, 0, 0);
  }
}

/********************* Switch/Button functions **************************/
static QueueHandle_t gpio_evt_queue = NULL;

static void onZbButton(SwitchData *button_func_pair) {
  if (button_func_pair->func == SWITCH_ONOFF_TOGGLE_CONTROL) {
    Serial.println("Button pressed - sending toggle command to bound lights");
    zbSwitch.lightToggle();
  }
}

static void IRAM_ATTR onGpioInterrupt(void *arg) {
  xQueueSendFromISR(gpio_evt_queue, (SwitchData *)arg, NULL);
}

static void enableGpioInterrupt(bool enabled) {
  for (int i = 0; i < PAIR_SIZE(buttonFunctionPair); ++i) {
    if (enabled) {
      enableInterrupt((buttonFunctionPair[i]).pin);
    } else {
      disableInterrupt((buttonFunctionPair[i]).pin);
    }
  }
}

/********************* Arduino functions **************************/
void setup() {
  // Enable accessory power first (required for LEDs)
  pinMode(ACCESSORY_POWER_PIN, OUTPUT);
  digitalWrite(ACCESSORY_POWER_PIN, HIGH);

  Serial.begin(9600);
  delay(100);
  Serial.println("\n\n=== ZigbeeBTN Starting ===");

  // Init WS2812 LED and turn it OFF
  Serial.println("Initializing WS2812 LED...");
  neopixelWrite(LED_PIN, 0, 0, 0);

  // Set Zigbee device name and model for light endpoint
  zbLight.setManufacturerAndModel("Apollo", "BTN-1");
  Serial.println("Zigbee Light manufacturer/model set");

  // Set callback function for light change
  zbLight.onLightChange(setLED);
  Serial.println("Light change callback registered");

  // Add light endpoint to Zigbee Core
  Serial.println("Adding ZigbeeLight endpoint to Zigbee Core...");
  Zigbee.addEndpoint(&zbLight);

  // Configure switch endpoint
  zbSwitch.setManufacturerAndModel("Apollo", "BTN-1-Switch");
  zbSwitch.allowMultipleBinding(true);
  Serial.println("Zigbee Switch manufacturer/model set");

  // Add switch endpoint to Zigbee Core
  Serial.println("Adding ZigbeeSwitch endpoint to Zigbee Core...");
  Zigbee.addEndpoint(&zbSwitch);

  // Init button for switch function
  for (int i = 0; i < PAIR_SIZE(buttonFunctionPair); i++) {
    pinMode(buttonFunctionPair[i].pin, INPUT_PULLUP);
    // Create a queue to handle gpio events from ISR
    gpio_evt_queue = xQueueCreate(10, sizeof(SwitchData));
    if (gpio_evt_queue == 0) {
      Serial.println("ERROR: Queue was not created!");
      while (1);
    }
    attachInterruptArg(buttonFunctionPair[i].pin, onGpioInterrupt, (void *)(buttonFunctionPair + i), FALLING);
  }
  Serial.println("Button interrupt configured");

  // When all EPs are registered, start Zigbee. By default acts as ZIGBEE_END_DEVICE
  Serial.println("Starting Zigbee...");
  Zigbee.begin();
  Serial.println("Zigbee.begin() called");

  // TEMPORARY: One-time factory reset to clear old network credentials
  // Remove this entire block after device is successfully paired!
  prefs.begin("zbtn", false);
  bool needsReset = prefs.getBool("reset3", true);  // Changed key to force new reset
  if (needsReset) {
    prefs.putBool("reset3", false);  // Mark as done
    prefs.end();
    Serial.println("Forcing one-time Zigbee factory reset...");
    neopixelWrite(LED_PIN, 255, 0, 0);  // Red = resetting
    delay(500);
    Zigbee.factoryReset();
    // Device will reboot, won't reach here
  }
  prefs.end();
  Serial.println("Waiting for network join...");
  neopixelWrite(LED_PIN, 0, 0, 255);  // Blue = waiting for pairing
}

void loop() {
  // Handle button switch events
  uint8_t pin = 0;
  SwitchData buttonSwitch;
  static SwitchState buttonState = SWITCH_IDLE;
  bool eventFlag = false;

  // Check if there is any queue received, if yes read out the buttonSwitch
  // Use 0 timeout to not block (allows factory reset check to work)
  if (xQueueReceive(gpio_evt_queue, &buttonSwitch, 0)) {
    pin = buttonSwitch.pin;
    enableGpioInterrupt(false);
    eventFlag = true;
  }

  while (eventFlag) {
    bool value = digitalRead(pin);
    switch (buttonState) {
      case SWITCH_IDLE:
        buttonState = (value == LOW) ? SWITCH_PRESS_DETECTED : SWITCH_IDLE;
        break;
      case SWITCH_PRESS_DETECTED:
        buttonState = (value == LOW) ? SWITCH_PRESS_DETECTED : SWITCH_RELEASE_DETECTED;
        break;
      case SWITCH_RELEASE_DETECTED:
        buttonState = SWITCH_IDLE;
        // Callback to button handler
        onZbButton(&buttonSwitch);
        break;
      default:
        break;
    }
    if (buttonState == SWITCH_IDLE) {
      enableGpioInterrupt(true);
      eventFlag = false;
      break;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  // Check for long press factory reset (3+ seconds)
  static unsigned long buttonPressStart = 0;
  static bool checkingLongPress = false;

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!checkingLongPress) {
      checkingLongPress = true;
      buttonPressStart = millis();
    } else if ((millis() - buttonPressStart) > 3000) {
      Serial.println("Resetting Zigbee to factory settings, reboot.");
      Zigbee.factoryReset();
    }
  } else {
    checkingLongPress = false;
  }

  delay(10);
}
