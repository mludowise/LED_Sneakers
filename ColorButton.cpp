#include "ColorButton.h"
#include "Arduino.h"
#include "ShoeAnimation.h"
#include "Util.h"
#include <EEPROM.h>

ColorButton:: ColorButton(int pin, int input, int threshold, int numColors, float colorOffset, ShoeAnimation* animationPtr, int eepromAddress) : 
  Button (pin, input, threshold), 
  NUM_COLORS(numColors), 
  COLOR_OFFSET(colorOffset), 
  COLOR_FREQUENCY(2 * PI / numColors), 
  mCurrentColorIndex(0), 
  mAnimationPtr(animationPtr),
  mEepromAddress(eepromAddress) {
    int savedColorIndex = EEPROM.read(mEepromAddress);
    if (savedColorIndex >= 0 && savedColorIndex <= NUM_COLORS) {
      mCurrentColorIndex = savedColorIndex;
    }
    updateAnimationColor();
}

void ColorButton::onUp() {
  ++mCurrentColorIndex %= (NUM_COLORS + 1);
  EEPROM.write(mEepromAddress, (byte) mCurrentColorIndex); // Save new index to EEPROM memory
  updateAnimationColor();
}

void ColorButton::updateAnimationColor() {
  if (mAnimationPtr) {
    bool isRainbow = mCurrentColorIndex >= NUM_COLORS;
    mAnimationPtr->setColor(isRainbow ? 0 : calculateColor(mCurrentColorIndex, COLOR_FREQUENCY, COLOR_OFFSET));
  }
}
