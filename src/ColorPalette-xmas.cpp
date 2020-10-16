#include <FastLED.h>
#include "main.h"

CRGB leds[NUM_LEDS];
CRGB led_buffer[NUM_LEDS];

MODE current_mode = DEMO;

void blank_leds() {
    memset(leds, CRGB::Black, NUM_LEDS);
}

void blank_led_buffer() {
    memset(led_buffer, CRGB::Black, NUM_LEDS);
}

void setup() {
    delay( 3000 ); // power-up safety delay
    
    blank_leds();
    blank_led_buffer();

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_CLOCK, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( 64 );

    Serial.begin(115200);
    demo_setup();
}

uint32_t last_show_time = 0;

void loop()
{
    check_commands();

    switch (current_mode) {
	    case DEMO:
		    demo_loop();
		    break;
	    case DEMO_SPECIFIC:
		    demo_specific_loop();
		    break;
	    case DEBUG:
		    debug_loop();
		    break;
	    case RAW:
		    // raw mode has no loop, it gets handled entirely by the serial driver
		    break;
	    default:
		    demo_setup();
		    current_mode = DEMO;
    }

	    FastLED.show();
}
