#include <FastLED.h>
#include "main.h"

#include <RingBuf.h>

// Allow us to have 2 full frames
RingBuf<uint8_t, 2 * (NUM_LEDS * 3) + 16> serial_data;

uint16_t buffer_position = 0;

enum SERIAL_COMMANDS : uint8_t {
  // raw mode commands
  // Puts us into raw mode, and blank the strand
  CMD_PIXELS_RAW_START = 0x01,
  // commands to start a drawing session
  CMD_PIXELS_START_DATA = 0x02, // give a position in the strand, start loading bulk data
  CMD_PIXELS_DATA = 0x03,
  CMD_PIXELS_DATA_VARIABLE = 0x04, // left for later todo
  CMD_PIXELS_SET_SINGLE = 0x05,
  // Commit the leds, copy the buffer
  CMD_PIXELS_COMMIT = 0x06,

  // let us set a global brightness value, and scale all calls to FastLED.setBrightness with it
  CMD_GENERIC_BRIGHTNESS = 0x40,

  CMD_DEMO_MODE = 0x70,
  CMD_DEMO_SPECIFIC = 0x71,

  // debug mode commands, set specific leds to colors and advance through them one at a time
  CMD_DEBUG_BRIGHT = 0xFC,
  CMD_DEBUG_POS = 0xFD,
  CMD_DEBUG_ADV = 0xFE,
  CMD_DEBUG_BLANK = 0xFF
};

void check_commands() {
  // TODO make this stop reading when the ring is full
  uint8_t temp;
  size_t ready = Serial.available();
  if (ready > 0) {
    while (ready > 0) {
      ready--;

      serial_data.push(Serial.read());
    }
  }

  uint16_t available = serial_data.size();

  if (available >= 1) {
    uint8_t cmd = serial_data[0];

    switch (cmd) {
      case CMD_DEBUG_POS: {
        if (available >= 3) {
          serial_data.pop(temp);
          uint8_t high, low;
          serial_data.pop(high);
          serial_data.pop(low);

          uint16_t position = (((uint16_t) high) << 8) + low;

          if (position >= NUM_LEDS) {
            Serial.println("Position beyond leds, ixmas_lightsgnoring");
          } else {
            debug_setup(position);
          }
        } else {
          Serial.println("Partial debug pos command in buffer");
        }
			}

      case CMD_DEBUG_ADV: {
        // simple command advance the state one step
        serial_data.pop(temp);
        advance_debug_state();
        break;
			}

      case CMD_DEBUG_BLANK: {
        serial_data.pop(temp);
        blank_leds();
        break;
			}

      case CMD_DEMO_MODE: {
        serial_data.pop(temp);
        current_mode = DEMO;
        demo_setup();
        break;
			}

      case CMD_DEMO_SPECIFIC: {
        if (available >= 2) {
          serial_data.pop(temp);
          uint8_t mode;
          serial_data.pop(mode);
          current_mode = DEMO_SPECIFIC;
          demo_specific_setup(mode);
        }
        break;
			}

      case CMD_PIXELS_RAW_START: {
        serial_data.pop(temp);
        current_mode = RAW;
        blank_leds();
        blank_led_buffer();
        break;
			}

      case CMD_PIXELS_START_DATA: {
        if (available >= 3) {
          serial_data.pop(temp);
          uint8_t high, low;
          serial_data.pop(high);
          serial_data.pop(low);

          buffer_position = (((uint16_t) high) << 8) + low;
        }
        break;
			}

      case CMD_PIXELS_DATA: {
        // this uses the full default buffer from an arduino
        // it then consumes the whole buffer
        if (available >= 64) {
          serial_data.pop(temp);

          // TODO make this use memcpy instead
          for (uint8_t i = 0; i < 64; i++) {
            // don't write past the buffer, but go ahead and
            uint8_t r, g, b;

            serial_data.pop(r);
            serial_data.pop(g);
            serial_data.pop(b);

            if (buffer_position < NUM_LEDS) {
              led_buffer[buffer_position].r = r;
              led_buffer[buffer_position].g = g;
              led_buffer[buffer_position].b = b;

              buffer_position++;
            }
          }
        }
        break;
			}

      case CMD_PIXELS_DATA_VARIABLE: {
        uint16_t pixel_count = (((uint16_t)serial_data[1]) << 8) + serial_data[2];
        if (available >= 5 + 3 * pixel_count) {
          // we have enough data, consume it off the buffer
          serial_data.pop(temp);
          serial_data.pop(temp);
          serial_data.pop(temp);

          uint8_t high, low;
          serial_data.pop(high);
          serial_data.pop(low);

          // TODO make this use memcpy instead
          for (uint16_t i = 0; i < pixel_count; i++) {
            // don't write past the buffer, but go ahead and
            uint8_t r, g, b;

            serial_data.pop(r);
            serial_data.pop(g);
            serial_data.pop(b);

            if (buffer_position < NUM_LEDS) {
              led_buffer[buffer_position].r = r;
              led_buffer[buffer_position].g = g;
              led_buffer[buffer_position].b = b;

              buffer_position++;
            }
          }
        }
        break;
			}

      case CMD_PIXELS_SET_SINGLE: {
        if (available >= 6) {
          serial_data.pop(temp);

          uint8_t high, low;
          serial_data.pop(high);
          serial_data.pop(low);

          uint16_t position = (((uint16_t) high) << 8) + low;

          uint8_t r, g, b;

          serial_data.pop(r);
          serial_data.pop(g);
          serial_data.pop(b);

          if (position < NUM_LEDS) {
            led_buffer[position].r = r;
            led_buffer[position].g = g;
            led_buffer[position].b = b;
          }
        }
        break;
			}

      case CMD_PIXELS_COMMIT: {
        serial_data.pop(temp);
        memcpy(leds, led_buffer, NUM_LEDS);
        break;
			}

      default: {
        Serial.println("Bad command in buffer, skipping");
        serial_data.pop(temp);
			}
		}
  }
}
