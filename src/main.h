#ifndef MAIN_H__
#define MAIN_H__ 1

#define LED_PIN     6
#define LED_CLOCK   7
#define NUM_LEDS    600
#define BRIGHTNESS  64
#define LED_TYPE    WS2801
#define COLOR_ORDER RGB
#define UPDATES_PER_SECOND 100

extern CRGB leds[];
extern CRGB led_buffer[];

enum MODE : uint8_t {
	DEMO,
	DEMO_SPECIFIC,
	DEBUG,
	RAW, // don't do any pixel updates ourselves
};

extern MODE current_mode;

void demo_setup();
void demo_loop();
void demo_specific_setup(uint8_t mode);
void demo_specific_loop();

void debug_setup(uint16_t position);
void debug_loop();

void check_commands();
void advance_debug_state();

void blank_leds();
void blank_led_buffer();

#endif
