#include <FastLED.h>
#include "main.h"

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupRedAndGreenPalette()
{
    CRGB red = CHSV( HUE_RED, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   red,  green,  black,  black,
                                   red, red, black,  black,
                                   green,  green,  black,  black,
                                   red, red, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Black,
    CRGB::White, // 'white' is too bright compared to red and blue
    CRGB::White, // 'white' is too bright compared to red and blue
    CRGB::White, // 'white' is too bright compared to red and blue
    CRGB::Black,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Red,
    CRGB::White, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
};



// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 360;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;

	demo_specific_setup(0x00);
        if( secondHand ==  0)   { demo_specific_setup(0x00); }
        if( secondHand == 90)   { demo_specific_setup(0x01); }
        if( secondHand == 120)  { demo_specific_setup(0x02); }
        if( secondHand == 210)  { demo_specific_setup(0x03); }
        if( secondHand == 240)  { demo_specific_setup(0x04); }
        if( secondHand == 270)  { demo_specific_setup(0x05); }
        if( secondHand == 330)  { demo_specific_setup(0x06); }
    }
}

// Additionl notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.

void demo_setup() {
    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
}

void demo_loop() {
    EVERY_N_MILLISECONDS(1000 / UPDATES_PER_SECOND) {
	    ChangePalettePeriodically();
	    
	    static uint8_t startIndex = 0;
	    startIndex = startIndex + 1; /* motion speed */
	    
	    FillLEDsFromPaletteColors( startIndex);
    }
}

void demo_specific_setup(uint8_t mode) {
    FastLED.setBrightness( BRIGHTNESS );

    switch(mode) {
	    case 0x00:
		    currentPalette = RainbowColors_p;
    		    currentBlending = LINEARBLEND;
		    break;
	    case 0x01:
		    currentPalette = RainbowStripeColors_p;
    		    currentBlending = LINEARBLEND;
		    break;
	    case 0x02:
        	    SetupRedAndGreenPalette();
    		    currentBlending = LINEARBLEND;
		    break;
	    case 0x03:
		    SetupBlackAndWhiteStripedPalette();
    		    currentBlending = LINEARBLEND;
		    break;
	    case 0x04:
		    currentPalette = CloudColors_p;
    		    currentBlending = LINEARBLEND;
		    break;
	    case 0x05:
		    currentPalette = PartyColors_p;
    		    currentBlending = LINEARBLEND;
		    break;
	    case 0x06: // TODO add more power taps so I can do this one at full bright
		    FastLED.setBrightness( 2 * BRIGHTNESS / 3);
		    currentPalette = myRedWhiteBluePalette_p;
    		    currentBlending = LINEARBLEND;
		    break;
	    case 0x07: // TODO see above
		    FastLED.setBrightness( 2 * BRIGHTNESS / 3);
		    currentPalette = HeatColors_p;
		    currentBlending = LINEARBLEND;
		    break;
	    case 0x08:
		    currentPalette = OceanColors_p;
		    currentBlending = LINEARBLEND;
		    break;
	    case 0x09:
		    currentPalette = LavaColors_p;
		    currentBlending = LINEARBLEND;
		    break;
	    case 0x0A:
		    currentPalette = ForestColors_p;
		    currentBlending = LINEARBLEND;
		    break;

	    default:
		    SetupRedAndGreenPalette();
    		    currentBlending = LINEARBLEND;
    }
}

void demo_specific_loop() {
    EVERY_N_MILLISECONDS(1000 / UPDATES_PER_SECOND) {
	    static uint8_t startIndex = 0;
	    startIndex = startIndex + 1; /* motion speed */
	    
	    FillLEDsFromPaletteColors( startIndex);
    }
}
