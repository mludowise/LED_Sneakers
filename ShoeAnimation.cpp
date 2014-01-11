#include "ShoeAnimation.h"
#include "Arduino.h"
#include "Util.h"

ShoeAnimation:: ShoeAnimation(int output, int numLEDs, int firstLEDIndex, int delayMillis) : 
  NUM_LEDS(numLEDs), 
  FIRST_LED_INDEX(firstLEDIndex), 
  RAINBOW_COLOR_FREQUENCY(2 * PI / numLEDs), 
  DELAY(delayMillis),
  mStrip(numLEDs, output, NEO_GRB + NEO_KHZ800), 
  mIsAnimating(false), 
  mTurnLEDsOn(true), 
  mNextLEDIndex(0), 
  mRainbowOffset(0), 
  mColor(0) {
    mStrip.begin();
    mStrip.show(); // Initialize all pixels to 'off'
}
      
void ShoeAnimation:: start() {
  mIsAnimating = true;
  mTurnLEDsOn = true;
  mNextLEDIndex = 0;
  mStartedWaiting = 0;
  if (mColor == 0) { // Currently displaying rainbow
    ++mRainbowOffset %= NUM_LEDS;     // Shift the rainbow for the next step
  }
  increment();
}

void ShoeAnimation:: increment() {
  if (!mIsAnimating) {
    return;
  }
  
  unsigned long now = millis();
  if (mStartedWaiting + DELAY <= now) { // If we've been waiting long enough for the next step in the animation
    uint32_t c = mTurnLEDsOn ? getLEDColor(mNextLEDIndex) : 0;
    
    setPixelColor(mNextLEDIndex, c);
    mStrip.show();
    
    if (++mNextLEDIndex >= NUM_LEDS) {
      if (mTurnLEDsOn) {
        mNextLEDIndex = 0;
        mTurnLEDsOn = false;
      } else {
        mIsAnimating = false;
      }
    }
    mStartedWaiting = now;
  }
}

void ShoeAnimation:: setColor(uint32_t c) {
  mColor = c;
  
  // Update all of the LEDs that are currently turned on to the appropriate color
  
  if (!mIsAnimating) { // Nothing to do here
    return;
  }
  
  int startIndex=  mTurnLEDsOn ? 0 : mNextLEDIndex; 
  int endIndex = mTurnLEDsOn ? mNextLEDIndex : NUM_LEDS;  
  
  for (int i = startIndex; i < endIndex; i++) {
    setPixelColor(i, getLEDColor(i));
  }
  mStrip.show();
}

void ShoeAnimation:: setPixelColor(int index, uint32_t color) {
  mStrip.setPixelColor((index + FIRST_LED_INDEX) % NUM_LEDS, color);
}

uint32_t ShoeAnimation:: getLEDColor(int index) {
  return mColor == 0 ? calculateColor(index + mRainbowOffset, RAINBOW_COLOR_FREQUENCY, 0) : mColor;
}

bool ShoeAnimation:: isAnimating() const {
  return mIsAnimating;
}
