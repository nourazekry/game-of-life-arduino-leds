#include <FastLED.h>

#define LED_PIN  5

#define COLOR_ORDER GRB
#define CHIPSET     WS2812B

#define BRIGHTNESS 64

CRGB blue = CRGB::Blue;
CRGB black = CRGB::Black;

// Helper functions for an two-dimensional XY matrix of pixels.
// Simple 2-D demo code is included as well.
//
//     XY(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;
//             No error checking is performed on the ranges of x and y.
//
//     XYsafe(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XYsafe(x,y) ] == CRGB::Red;
//             Error checking IS performed on the ranges of x and y, and an
//             index of "-1" is returned.  Special instructions below
//             explain how to use this without having to do your own error
//             checking every time you use this function.  
//             This is a slightly more advanced technique, and 
//             it REQUIRES SPECIAL ADDITIONAL setup, described below.


// Params for width and height
const uint8_t kMatrixWidth = 12;
const uint8_t kMatrixHeight = 12;

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;
const bool    kMatrixVertical = false;

// Set 'kMatrixSerpentineLayout' to false if your pixels are 
// laid out all running the same way, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//     .----<----<----<----'
//     |
//     5 >  6 >  7 >  8 >  9
//                         |
//     .----<----<----<----'
//     |
//    10 > 11 > 12 > 13 > 14
//                         |
//     .----<----<----<----'
//     |
//    15 > 16 > 17 > 18 > 19
//

//
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * kMatrixWidth) + x;
    } else {
      i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
    }
  }

  if( kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if( y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (kMatrixWidth - 1) - x;
        i = (y * kMatrixWidth) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * kMatrixWidth) + x;
      }
    } else { // vertical positioning
      if ( x & 0x01) {
        i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
      } else {
        i = kMatrixHeight * (kMatrixWidth - x) - (y+1);
      }
    }
  }
  
  return i;
}



#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

void DrawFirstFrame() {
  uint8_t i, j;
  for ( i = 0; i < kMatrixHeight; i++) {
    for ( j = 0; j < kMatrixWidth; j++) {
      if(rand() % 2 == 0) {
         leds[XYsafe(i, j)] = CRGB::Black;
      } else {
        leds[XYsafe(i, j)] = CRGB::Blue;
      } 
    }
  } 
}

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}


void DrawOneFrame()
{
  uint8_t n, i, j, k, l;
DrawFirstFrame();  

for(n = 0; n < 6; n++) {
  FastLED.show();
delay(50);
  for ( i = 0; i < kMatrixHeight; i++) {
    for ( j = 0; j < kMatrixWidth; j++) {
      int living = 0;
      for ( l = -1; l <= 1; l++) {
                for ( k = -1;  k <= 1; k++) {
                    // check bounds of grid
                    // count living neighbors
                    if ((((i + l) >= 0) && ((i + l) < kMatrixHeight)) && (((j + k) >= 0) && ((j + k) < kMatrixWidth))) {
                        if(leds[ XYsafe(i+l,j+k) ] == blue) {
                            living++;
                        }
                    }
                }
            }

                if (leds[ XYsafe(i,j) ] == blue) {
                    // living cell
                    // subtracts one to remove current cell from neighbor count
                    living--;
                    if (living == 2 || living == 3) {
                        leds[XYsafe(i,j)] = CRGB::Blue;
                    } else {
                        leds[XYsafe(i,j)] = CRGB::Black;
                    }
                } else {
                    if (living == 3) {
                        // dead cell with 3 living neighbors
                        
                        leds[XYsafe(i,j)] = CRGB::Blue;
                    }
                }
            
    }
  }
}
  
}
void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
}

void loop()
{
    DrawOneFrame();
    FastLED.show();
}
