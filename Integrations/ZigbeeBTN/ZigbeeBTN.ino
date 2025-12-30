#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "ZigbeeCore.h"
#include "ep/ZigbeeLight.h"

#define LED_PIN               21
#define BUTTON_PIN            9  // ESP32-C6/H2 Boot button
#define ZIGBEE_LIGHT_ENDPOINT 10

// WS2812 LED color when ON (white at moderate brightness)
#define LED_BRIGHTNESS        64
#define LED_R                 LED_BRIGHTNESS
#define LED_G                 LED_BRIGHTNESS
#define LED_B                 LED_BRIGHTNESS

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

/********************* RGB LED functions **************************/
void setLED(bool value) {
  Serial.printf("setLED callback called with value: %s\n", value ? "ON" : "OFF");
  if (value) {
    neopixelWrite(LED_PIN, LED_R, LED_G, LED_B);
  } else {
    neopixelWrite(LED_PIN, 0, 0, 0);
  }
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("\n\n=== ZigbeeBTN Starting ===");

  // Init WS2812 LED and turn it OFF
  Serial.println("Initializing WS2812 LED...");
  neopixelWrite(LED_PIN, 0, 0, 0);

  // Init button for factory reset
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Set Zigbee device name and model
  zbLight.setManufacturerAndModel("Apollo", "BTN-1");
  Serial.println("Zigbee manufacturer/model set");

  // Set callback function for light change
  zbLight.onLightChange(setLED);
  Serial.println("Light change callback registered");

  // Add endpoint to Zigbee Core
  Serial.println("Adding ZigbeeLight endpoint to Zigbee Core...");
  Zigbee.addEndpoint(&zbLight);

  // When all EPs are registered, start Zigbee. By default acts as ZIGBEE_END_DEVICE
  Serial.println("Starting Zigbee...");
  Zigbee.begin();
  Serial.println("Zigbee.begin() called - waiting for network join");
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(BUTTON_PIN) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.printf("Resetting Zigbee to factory settings, reboot.\n");
        Zigbee.factoryReset();
      }
    }
  }
  delay(100);
}