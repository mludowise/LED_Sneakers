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

// Constants used to calculate rainbow effect
#define PI2_3 2.09439510239    // = 2 * PI / 3;
#define PI4_3 4.18879020479    // = 4 * PI / 3;

class Button {
  private:
    Button(const Button& copy); 
    Button& operator=(const Button& copy); 

    const int mPin;
    const int mThreshold;
    bool mIsDown;
  
  protected:
    Button(int pin, int input, int threshold) :
      mPin(pin), mThreshold(threshold), mIsDown(false) {
        pinMode(input, INPUT_PULLUP);
      }
      
    virtual void onDown() { };     // Triggered when button is pushed down
    virtual void onUp() { };       // Triggered when button is released
    
  public:
    void checkButtonState() {
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
    
    bool isDown() const {
      return mIsDown;
    }
};

class ShoeAnimation {
  private:
    ShoeAnimation(const ShoeAnimation& copy); 
    ShoeAnimation& operator=(const ShoeAnimation& copy); 
    
    const int mNumLEDs;
    const int mFirstLED;
    const float mRainbowFrequency; // Use this frequency when displaying rainbow effect. The shoe will show one rainbow cycle around its circumfrence.
    
    Adafruit_NeoPixel mStrip;
    bool mIsAnimating; 
    bool mTurnLEDsOn;
    int mNextLEDIndex;
    unsigned long mStartedWaiting;
    int mRainbowShift;
    uint32_t mColor;
    
  public: 
    ShoeAnimation(int output, int numLEDs, int firstLED) : 
      mNumLEDs(numLEDs), mFirstLED(firstLED), mRainbowFrequency(2 * PI / numLEDs),
      mStrip(numLEDs, output, NEO_GRB + NEO_KHZ800), mIsAnimating(false), mTurnLEDsOn(true), mNextLEDIndex(0), mRainbowShift(0), mColor(0) {
        mStrip.begin();
        mStrip.show(); // Initialize all pixels to 'off'
      }
      
      void startAnimation() {
        mIsAnimating = true;
        mTurnLEDsOn = true;
        mNextLEDIndex = 0;
        mStartedWaiting = 0;
        if (mColor == 0) { // Currently displaying rainbow
          ++mRainbowShift %= mNumLEDs;     // Shift the rainbow for the next step
        }
        incrementAnimation();
      }
      
      void incrementAnimation() {
        unsigned long now = millis();
        if (mStartedWaiting + WAIT <= now) { // If we've been waiting long enough for the next step in the animation
          changeNextLED();
          mStartedWaiting = now;
        }
      }
      
      void changeNextLED() {
        uint32_t nextLEDColor = 0;
        if (mTurnLEDsOn) {
          if (mColor == 0) {
            nextLEDColor = calculateColor(mNextLEDIndex + mRainbowShift, mRainbowFrequency, 0);
          } else {
            nextLEDColor = mColor;
          }
        }
  
        #if TEST == 1
        Serial.print((mColor & 0xFF0000) >> 16);
        Serial.print("\t");
        Serial.print((mColor & 0x00FF00) >> 8);
        Serial.print("\t");
        Serial.println(mColor & 0x0000FF);
        #endif
  
        mStrip.setPixelColor((mNextLEDIndex + mFirstLED) % mNumLEDs, nextLEDColor);
        mStrip.show();
  
        mNextLEDIndex++; // Increment LED
        if (mNextLEDIndex >= mNumLEDs) {
          if (mTurnLEDsOn) {
            mNextLEDIndex = 0;
            mTurnLEDsOn = false;
          } else {
            mIsAnimating = false;
          }
        }
      }
      
      bool isAnimating() const {
        return mIsAnimating;
      }
      
      void setColor(uint32_t c) {
        mColor = c;
      }
};

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
        mAnimationPtr->setColor(isRainbow ? 0 : calculateColor(mCurrentColorIndex, mFrequency, mColorOffset));
      }
    }
};

ShoeAnimation* animation = NULL;
ColorButton* colorButton = NULL;
Button* stepButton = NULL;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  animation = new ShoeAnimation(LED_OUTPUT, NUM_LEDS, FIRST_LED);
  colorButton = new ColorButton(COLOR_IN_PIN, COLOR_INPUT, 100, NUM_COLORS, COLOR_OFFSET, animation);
  stepButton = new StepButton(STEP_IN_PIN, STEP_INPUT, STEP_SENSOR_TRIGGER);
}
 
void loop() {
  // Check for inputs
  colorButton->checkButtonState();
  stepButton->checkButtonState();
    
  if (animation->isAnimating()) { // Are we in the process of an LED animation?
    animation->incrementAnimation();
  } else if (stepButton->isDown()) { // Is someone stepping?
    animation->startAnimation();
  }
}

// Referenced from http://krazydad.com/tutorials/makecolors.php
static uint32_t calculateColor(int i, float frequency, float offset) {
 return Adafruit_NeoPixel::Color(
      sin(frequency * (i + offset)) * 127 + 128,
      sin(frequency * (i + offset) + PI2_3) * 127 + 128,
      sin(frequency * (i + offset) + PI4_3) * 127 + 128
    ); 
}
