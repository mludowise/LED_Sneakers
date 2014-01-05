#include "ShoeAnimation.h"
#include "Arduino.h"
#include "Util.h"

ShoeAnimation:: ShoeAnimation(int output, int numLEDs, int firstLEDIndex, int delayMillis) : 
  NUM_LEDS(numLEDs), FIRST_LED_INDEX(firstLEDIndex), RAINBOW_COLOR_FREQUENCY(2 * PI / numLEDs), DELAY(delayMillis),
  mStrip(numLEDs, output, NEO_GRB + NEO_KHZ800), mIsAnimating(false), mTurnLEDsOn(true), mNextLEDIndex(0), mRainbowOffset(0), mColor(0) {
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
    changeNextLED();
    mStartedWaiting = now;
  }
}

void ShoeAnimation:: changeNextLED() {
  if (mTurnLEDsOn) {
    turnOnLED(mNextLEDIndex);
  } else {
    turnOffLED(mNextLEDIndex);
  }
  
  mNextLEDIndex++; // Increment LED
  if (mNextLEDIndex >= NUM_LEDS) {
    if (mTurnLEDsOn) {
      mNextLEDIndex = 0;
      mTurnLEDsOn = false;
    } else {
      mIsAnimating = false;
    }
  }
}

void ShoeAnimation:: turnOnLED(int index) {
  uint32_t nextLEDColor = 0;
  if (mColor == 0) {
    nextLEDColor = Util::calculateColor(mNextLEDIndex + mRainbowOffset, RAINBOW_COLOR_FREQUENCY, 0);
  } else {
    nextLEDColor = mColor;
  }
  mStrip.setPixelColor((index + FIRST_LED_INDEX) % NUM_LEDS, nextLEDColor);
  mStrip.show();
}

void ShoeAnimation:: turnOffLED(int index) {
  mStrip.setPixelColor((index + FIRST_LED_INDEX) % NUM_LEDS, 0);
  mStrip.show();
}

bool ShoeAnimation:: isAnimating() const {
  return mIsAnimating;
}

void ShoeAnimation:: setColor(uint32_t c) {
  mColor = c;
  
  // Update all of the LEDs that are currently turned on to the appropriate color
  
  if (!mIsAnimating) { // Nothing to do here
    return;
  }
  
  int startIndex, endIndex;
  if (mTurnLEDsOn) { // We're in the process of turning LEDs on
    startIndex = 0;
    endIndex = mNextLEDIndex;
  } else { // We're in the process of turning LEDs off
    startIndex = mNextLEDIndex;
    endIndex = NUM_LEDS;
  }
  
  for (int i = startIndex; i < endIndex; i++) {
    turnOnLED(i);
  }
}
