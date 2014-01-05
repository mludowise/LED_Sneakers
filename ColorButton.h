#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include "Button.h"
#include "ShoeAnimation.h"

class ColorButton : public Button {
  private:
    const int NUM_COLORS;          // The number of colors to cycle through (not including rainbow mode)
    const float COLOR_FREQUENCY;   // Use this frequency to calculate color of the LEDs when cycling through the colors.
    const float COLOR_OFFSET;      // Value between 0 and 1, used to tweak the exact colors that are cycled through
    int mCurrentColorIndex;        // Denotes the index of the color currently being used
    ShoeAnimation* mAnimationPtr;  // Pointer to ShoeAnimation whose color we're changing
    
  public:
    // Constructor: pin number, input number, threshold the value of the input must dip below to trigger a button push
    // number of colors to cycle through, color offset value from 0 to 1, pointer to ShoeAnimation we're changing the color of
    ColorButton(int pin, int input, int threshold, int numColors, float colorOffset, ShoeAnimation* animationPtr);
  
  private:
    // Change color when button is released
    void onUp();
    
    // Do nothing when button is pushed down
    void onDown() { }
    
    // Updates the color of the ShowAnimation object
    void updateAnimationColor();
};
#endif
