#include "Arduino.h"
#include "Button.h"

Button:: Button(int pin, int input, int threshold) :
  PIN(pin), 
  THRESHOLD(threshold), 
  mIsDown(false),
  mPrint(false) {
    pinMode(input, INPUT_PULLUP);
}
    
void Button:: checkButtonState() {
  int sensorValue = analogRead(PIN);
  
  if (mPrint) {
    Serial.print("Sensor value = ");
    Serial.println(sensorValue);
  }
  
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

void Button:: printSensorValue(bool b) {
  mPrint = b;
}
