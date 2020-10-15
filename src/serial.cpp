#include <FastLED.h>
#include "main.h"

enum SERIAL_COMMANDS : uint8_t {
  // raw mode commands
  // Puts us into raw mode, and blank the strand
  CMD_PIXELS_RAW_START = 0x01,
  // commands to start a drawing session
  CMD_PIXELS_START_DATA = 0x02, // give a position in the strand, start loading bulk data
  CMD_PIXELS_DATA = 0x03,
  CMD_PIXELS_DATA_PARTIAL = 0x04,
  CMD_PIXELS_SET_SINGLE = 0x05,
  // Commit the leds, copy the buffer
  CMD_PIXELS_END = 0x06,

  
  CMD_DEMO_MODE = 0x70,
  CMD_DEMO_SPECIFIC = 0x71,

  // debug mode commands, set specific leds to colors and advance through them one at a time
  CMD_DEBUG_BRIGHT = 0xFC,
  CMD_DEBUG_POS = 0xFD,
  CMD_DEBUG_ADV = 0xFE,
  CMD_DEBUG_BLANK = 0xFF
};

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
					 debug_setup(position);
				 }
			 } else {
				 Serial.println("Partial debug pos command in buffer");
			 }
		  case CMD_DEBUG_ADV:
			 // simple command advance the state one step
			 handle_cmd();
			 advance_debug_state();
			 break;
		  case CMD_DEBUG_BLANK:
			 handle_cmd();
			 blank_leds();
			 break;
		  default:
		  Serial.println("Bad command in buffer, skipping");
		  handle_cmd();
	  }


  }
}
