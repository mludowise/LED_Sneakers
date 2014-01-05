#ifndef STEP_BUTTON_H
#define STEP_BUTTON_H

#include "Button.h"
#include "ShoeAnimation.h"

class StepButton : public Button {
  private:
    const bool TRIGGER_ONCE_PER_STEP;
    ShoeAnimation* mAnimationPtr;  // Pointer to ShoeAnimation whose color we're changing
    
  public:
    // Constructor: pin number, input number, threshold the value of the input must dip below to trigger a button push,
    // pointer to ShoeAnimation we're changing the color of
    StepButton(int pin, int input, int threshold, bool triggerOncePerStep, ShoeAnimation* animationPtr);
  
  private:
    // Do nothing when heal is released
    void onUp() { }
    
    // Start animation when heal is pushed down
    void onDown();
};
#endif
