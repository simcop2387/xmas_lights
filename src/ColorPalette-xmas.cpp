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
    switch(cstate) {
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
		    nstate = RED;
		    break;
	}	
    cstate = nstate;
}

#define handle_cmd() {Serial.read();}

void check_commands() {
  uint16_t available = Serial.available();

  if (available >= 1) {
	  uint8_t cmd = Serial.peek();

	  switch(cmd) {
		  case CMD_DEBUG_POS:
			 if (available >= 3) {
				 handle_cmd();
				 uint8_t high, low;
				 high = Serial.read();
				 low = Serial.read();

				 uint16_t position = (high << 8) + low;
				 
				 if (position >= NUM_LEDS) {
					 Serial.println("Position beyond leds, ignoring");
				 } else {
					 // position set, blank it, get ready to advance
					 leds[current_led_idx] = CRGB::Black;
					 current_led_idx = position;
					 cstate = BLANK;
					 nstate = RED;
				 }
			 } else {
				 Serial.println("Partial debug pos command in buffer");
			 }
		  case CMD_DEBUG_ADV:
			 // simple command advance the state one step
			 handle_cmd();
			 advance_state();
			 break;
		  case CMD_DEBUG_BLANK:
			 handle_cmd();
			 memset(leds, CRGB::Black, NUM_LEDS);
			 break;
		  default:
		  Serial.println("Bad command in buffer, skipping");
		  handle_cmd();
	  }


  }
}

void setup() {
    delay( 3000 ); // power-up safety delay
    memset(leds, CRGB::Black, NUM_LEDS);
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_CLOCK, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    Serial.begin(115200);
}

uint32_t last_show_time = 0;

void loop()
{
    check_commands();

	    FastLED.show();
}
