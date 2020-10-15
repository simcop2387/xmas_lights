#include <FastLED.h>
#include "main.h"

uint16_t current_led_idx = 0;

enum DEBUG_STATE : uint8_t {
	RED,
	GREEN,
	BLUE,
	WHITE,
	CYAN,
	MAGENTA,
	YELLOW,
	BLANK,
};

#define edge(color, next) dnstate = next; leds[current_led_idx] = color; break;

DEBUG_STATE dcstate = BLANK,
      dnstate = RED;

void advance_debug_state() {
    switch(dcstate) {
            case RED:
                    leds[current_led_idx] = CRGB::Black;
                    current_led_idx = (current_led_idx + 1) % NUM_LEDS;

		    edge(CRGB::Red, GREEN);
            case GREEN:
		    edge(CRGB::Green, BLUE);
            case BLUE:
		    edge(CRGB::Blue, WHITE);
	    case WHITE:
		    edge(CRGB::White, CYAN);
            case CYAN:
		    edge(CRGB::Cyan, MAGENTA);
            case MAGENTA:
		    edge(CRGB::Magenta, YELLOW);
            case YELLOW:
		    edge(CRGB::Yellow, RED);
	    default:
		    dnstate = RED;
		    break;
	}	
    dcstate = dnstate;
}

void debug_setup(uint16_t position) {
  blank_leds();
  leds[current_led_idx] = CRGB::Black;
  current_led_idx = position;
  dcstate = BLANK;
  dnstate = RED;
}

void debug_loop() {
  // nothing to do, just let the serial comms deal with it
}
