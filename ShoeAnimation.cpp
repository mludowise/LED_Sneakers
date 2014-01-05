#include "ShoeAnimation.h"
#include "Arduino.h"
#include "Util.h"

ShoeAnimation:: ShoeAnimation(int output, int numLEDs, int firstLEDIndex) : 
  mNumLEDs(numLEDs), mFirstLEDIndex(firstLEDIndex), mRainbowFrequency(2 * PI / numLEDs),
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
    ++mRainbowOffset %= mNumLEDs;     // Shift the rainbow for the next step
  }
  increment();
}

void ShoeAnimation:: increment() {
  if (!mIsAnimating) {
    return;
  }
  
  unsigned long now = millis();
  if (mStartedWaiting + WAIT <= now) { // If we've been waiting long enough for the next step in the animation
    changeNextLED();
    mStartedWaiting = now;
  }
}

void ShoeAnimation:: changeNextLED() {
  uint32_t nextLEDColor = 0;
  if (mTurnLEDsOn) {
    if (mColor == 0) {
      nextLEDColor = Util::calculateColor(mNextLEDIndex + mRainbowOffset, mRainbowFrequency, 0);
    } else {
      nextLEDColor = mColor;
    }
  }
  
  #if TEST == 1
  Serial.print((mColor & 0xFF0000) >> 16);
  Serial.print("\t");
  Serial.print((mColor & 0x00FF00) >> 8);
  Serial.print("\t");
  Serial.println(mColor & 0x0000FF);
  #endif
  
  mStrip.setPixelColor((mNextLEDIndex + mFirstLEDIndex) % mNumLEDs, nextLEDColor);
  mStrip.show();
  
  mNextLEDIndex++; // Increment LED
  if (mNextLEDIndex >= mNumLEDs) {
    if (mTurnLEDsOn) {
      mNextLEDIndex = 0;
      mTurnLEDsOn = false;
    } else {
      mIsAnimating = false;
    }
  }
}

bool ShoeAnimation:: isAnimating() const {
  return mIsAnimating;
}

void ShoeAnimation:: setColor(uint32_t c) {
  mColor = c;
}
