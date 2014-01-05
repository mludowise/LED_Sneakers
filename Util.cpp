#include "Util.h"
#include "Arduino.h"

// Referenced from http://krazydad.com/tutorials/makecolors.php
// Returns a color from a specific index, i, in the rainbow based on a frequency such that frequency = 2 * PI / total_indices
uint32_t calculateColor(int i, float frequency, float offset) {
 return Adafruit_NeoPixel::Color(
      sin(frequency * (i + offset)) * 127 + 128,
      sin(frequency * (i + offset) + PI2_3) * 127 + 128,
      sin(frequency * (i + offset) + PI4_3) * 127 + 128
    ); 
}
