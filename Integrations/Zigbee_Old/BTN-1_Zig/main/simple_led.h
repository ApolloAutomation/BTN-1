#ifndef SIMPLE_LED_H
#define SIMPLE_LED_H

// Simple LED patterns
typedef enum {
    LED_PATTERN_OFF,
    LED_PATTERN_PAIRING,       // Fast blink
    LED_PATTERN_JOINING,        // Slow blink
    LED_PATTERN_CONNECTED,      // Solid on for 3 seconds
    LED_PATTERN_ERROR,          // Double blink
    LED_PATTERN_FACTORY_RESET   // Rapid flash
} led_pattern_t;

// Initialize simple LED system
void simple_led_init(void);

// Set LED pattern
void simple_led_set_pattern(led_pattern_t pattern);

// Quick flash for button press acknowledgment
void simple_led_indicate_button_press(void);

#endif // SIMPLE_LED_H