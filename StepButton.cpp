#include "StepButton.h"
#include "ShoeAnimation.h"

StepButton:: StepButton(int pin, int input, int threshold, bool triggerOncePerStep, ShoeAnimation* animationPtr) :
  Button(pin, input, threshold),
  TRIGGER_ONCE_PER_STEP(triggerOncePerStep),
  mAnimationPtr(animationPtr) { }
  
void StepButton:: onDown() {
  if (TRIGGER_ONCE_PER_STEP && mAnimationPtr
      && !mAnimationPtr->isAnimating()) {
    mAnimationPtr->start();
  }
}

