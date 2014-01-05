#include "Arduino.h"
#include "Button.h"

Button:: Button(int pin, int input, int threshold) :
  mPin(pin), mThreshold(threshold), mIsDown(false) {
    pinMode(input, INPUT_PULLUP);
}
    
void Button:: checkButtonState() {
  int sensorValue = analogRead(mPin);
  if (sensorValue < mThreshold) { // Pressed
    bool wasDown = mIsDown;
    mIsDown = true;
    if (!wasDown) {
      onDown();
    }
  } else if (mIsDown) { // Released
    mIsDown = false;
    onUp();
  }
}
    
bool Button:: isDown() const {
  return mIsDown;
}