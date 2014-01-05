#include "Arduino.h"
#include "Button.h"

Button:: Button(int pin, int input, int threshold) :
  PIN(pin), THRESHOLD(threshold), mIsDown(false) {
    pinMode(input, INPUT_PULLUP);
}
    
void Button:: checkButtonState() {
  int sensorValue = analogRead(PIN);
  if (sensorValue < THRESHOLD) { // Pressed
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
