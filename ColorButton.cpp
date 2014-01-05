#include "ColorButton.h"
#include "Arduino.h"
#include "ShoeAnimation.h"
#include "Util.h"

ColorButton:: ColorButton(int pin, int input, int threshold, int numColors, float colorOffset, ShoeAnimation* animationPtr) : 
  Button (pin, input, threshold), mNumColors(numColors), mColorOffset(colorOffset), mFrequency(2 * PI / numColors), mCurrentColorIndex(0), mAnimationPtr(animationPtr) {
    updateAnimationColor();
}

void ColorButton::onUp() {
  ++mCurrentColorIndex %= (mNumColors + 1);
  updateAnimationColor();
}

void ColorButton::updateAnimationColor() {
  bool isRainbow = mCurrentColorIndex == mNumColors;
  if (mAnimationPtr) {
    mAnimationPtr->setColor(isRainbow ? 0 : Util::calculateColor(mCurrentColorIndex, mFrequency, mColorOffset));
  }
}
