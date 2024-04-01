#include <Adafruit_NeoPixel.h>

#define PIN        10  // Pin connected to the NeoPixels
#define NUMPIXELS  70  // Number of NeoPixels in the strip

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin(); // Initialize NeoPixel strip
  strip.clear(); // Turn off all pixels
  strip.show();  // Send the updated pixel color to the hardware
}

void loop() {
  // Alternate between red and white for adjacent pixels
  for(int i = 0; i < NUMPIXELS; i++) {
    if(i % 2 == 0) {
      // Set even-indexed pixels to red
      strip.setPixelColor(i, strip.Color(64, 0, 0));
    } else {
      // Set odd-indexed pixels to white
      strip.setPixelColor(i, strip.Color(64, 64, 64));
    }
  }
  strip.show(); // Update strip with new colors
  delay(1000);   // Adjust delay as needed
  
  // Shift the colors by one pixel to the right
  uint32_t lastColor = strip.getPixelColor(NUMPIXELS - 1);
  for(int i = NUMPIXELS - 1; i > 0; i--) {
    strip.setPixelColor(i, strip.getPixelColor(i - 1));
  }
  strip.setPixelColor(0, lastColor);
  strip.show(); // Update strip with new colors
  delay(1000);   // Adjust delay as needed
}
