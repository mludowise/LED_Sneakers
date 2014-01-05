#include "Button.h"
#include "ShoeAnimation.h"
#include "Util.h"
#include <Adafruit_NeoPixel.h>


#define TEST 2            // If we should print test statements (1 = explicit, 2 = info)

// Define all our inputs and outputs
#define STEP_IN_PIN A9    // Analog input pin that the step potentiometer is attached to
#define STEP_INPUT 9      // Analog input pin that the step potentiometer is attached to
#define COLOR_IN_PIN A10  // Analog input pin that the color potentiometer is attached to
#define COLOR_INPUT 10    // Analog input pin that the color potentiometer is attached to
#define LED_OUTPUT 6      // Output pin that the Neo Pixel LEDs are attached to

// Set these based on individual shoe and potentiometer sensitivies
#define STEP_SENSOR_TRIGGER 30  // The LEDs will be triggered when the step input goes below this value
#define NUM_LEDS 25             // The number of LEDs on the shoe
#define FIRST_LED 0             // The index of the first LED on the shoe to start the cycle with
#define NUM_COLORS 8            // The number of colors to cycle through with button clicks
#define COLOR_OFFSET 0.8f       // Value between 0 and 1, used to tweak the exact colors that are cycled through

// Some constants used for display
#define WAIT 25             // Time delay to wait for LEDs to light up

class ColorButton : public Button {
  private:
    const int mNumColors;
    const float mFrequency; // Use this frequency to calculate color of the LEDs when cycling through the colors.
    const float mColorOffset;
    int mCurrentColorIndex;
    ShoeAnimation* mAnimationPtr;
    
  public:
    ColorButton(int pin, int input, int threshold, int numColors, float colorOffset, ShoeAnimation* animationPtr) : 
      Button (pin, input, threshold), mNumColors(numColors), mColorOffset(colorOffset), mFrequency(2 * PI / numColors), mCurrentColorIndex(0), mAnimationPtr(animationPtr) {
        updateAnimationColor();
      }
  
  private:
    void onUp() {
      ++mCurrentColorIndex %= (mNumColors + 1);
      updateAnimationColor();
    }
    
    void onDown() { }
    
    void updateAnimationColor() {
      bool isRainbow = mCurrentColorIndex == mNumColors;
      if (mAnimationPtr) {
        mAnimationPtr->setColor(isRainbow ? 0 : Util::calculateColor(mCurrentColorIndex, mFrequency, mColorOffset));
      }
    }
};

ShoeAnimation* animation = NULL;
ColorButton* colorButton = NULL;
Button* stepButton = NULL;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  animation = new ShoeAnimation(LED_OUTPUT, NUM_LEDS, FIRST_LED, WAIT);
  colorButton = new ColorButton(COLOR_IN_PIN, COLOR_INPUT, 100, NUM_COLORS, COLOR_OFFSET, animation);
  stepButton = new Button(STEP_IN_PIN, STEP_INPUT, STEP_SENSOR_TRIGGER);
}
 
void loop() {
  // Check for inputs
  colorButton->checkButtonState();
  stepButton->checkButtonState();
    
  if (animation->isAnimating()) { // Are we in the process of an LED animation?
    animation->increment();
  } else if (stepButton->isDown()) { // Is someone stepping?
    animation->start();
    animation->increment();
  }
}
