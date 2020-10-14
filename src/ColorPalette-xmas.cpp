#include <FastLED.h>

#define LED_PIN     6
#define LED_CLOCK   7
#define NUM_LEDS    1024
#define BRIGHTNESS  255
#define LED_TYPE    WS2801
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// debug mode commands, set specific leds to colors and advance through them one at a time
#define CMD_DEBUG_POS 0xFD
#define CMD_DEBUG_ADV 0xFE
#define CMD_DEBUG_BLANK 0xFF

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

uint16_t current_led_idx = 0;

enum STATE {
	RED,
	GREEN,
	BLUE,
	WHITE,
	CYAN,
	MAGENTA,
	YELLOW,
	BLANK,
};

#define edge(color, next) nstate = next; leds[current_led_idx] = color; break;

STATE cstate = BLANK,
      nstate = RED;

void advance_state() {
    cstate = nstate;
    switch(cstate) {
            case RED:
                    leds[current_led_idx] = CRGB::Black;
                    current = (current + 1) % NUM_LEDS;

		    edge(CRGB::Red, GREEN);
            case GREEN:
		    edge(CRGB::Red, BLUE);
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
		    nstate = RED;
		    break;
	}	
}

void check_commands() {

}

void setup() {
    delay( 3000 ); // power-up safety delay
    memset(leds, CRGB::Black, NUM_LEDS);
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_CLOCK, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    Serial.begin(115200);
}

void loop()
{
    advance_state();

    FastLED.show();
}
