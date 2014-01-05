#include "ColorButton.h"
#include "Arduino.h"
#include "ShoeAnimation.h"
#include "Util.h"

ColorButton:: ColorButton(int pin, int input, int threshold, int numColors, float colorOffset, ShoeAnimation* animationPtr) : 
  Button (pin, input, threshold), NUM_COLORS(numColors), COLOR_OFFSET(colorOffset), COLOR_FREQUENCY(2 * PI / numColors), mCurrentColorIndex(0), mAnimationPtr(animationPtr) {
    updateAnimationColor();
}

void ColorButton::onUp() {
  ++mCurrentColorIndex %= (NUM_COLORS + 1);
  updateAnimationColor();
}

void ColorButton::updateAnimationColor() {
  bool isRainbow = mCurrentColorIndex == NUM_COLORS;
  if (mAnimationPtr) {
    mAnimationPtr->setColor(isRainbow ? 0 : calculateColor(mCurrentColorIndex, COLOR_FREQUENCY, COLOR_OFFSET));
  }
}
