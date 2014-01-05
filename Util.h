#ifndef UTIL_H
#define UTIL_H

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define PI2_3 2.09439510239    // = 2 * PI / 3;
#define PI4_3 4.18879020479    // = 4 * PI / 3;

uint32_t calculateColor(int i, float frequency, float offset);


#endif
