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
static const float RAINBOW_EFFECT_FREQUENCY = 2 * PI / NUM_LEDS;  // Use this frequency when displaying rainbow effect. The shoe will show one rainbow cycle around its circumfrence.
static const float SINGLE_COLOR_FREQUENCY = 2 * PI / NUM_COLORS;  // Use this frequency to calculate color of the LEDs when cycling through the colors.
static const float PI2_3 = 2 * PI / 3;
static const float PI4_3 = 4 * PI / 3;

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
      
    virtual void onDown() = 0;
    virtual void onUp() = 0;
    
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
    
    Adafruit_NeoPixel mStrip;
    bool mIsAnimating; 
    bool mTurnLEDsOn;
    int mNextLEDIndex;
    unsigned long mStartedWaiting;
    int mRainbowShift;
    uint32_t mColor;
    
  public: 
    ShoeAnimation() : 
      mStrip(NUM_LEDS, LED_OUTPUT, NEO_GRB + NEO_KHZ800), mIsAnimating(false), mTurnLEDsOn(true), mNextLEDIndex(0), mRainbowShift(0), mColor(0) {
        mStrip.begin();
        mStrip.show(); // Initialize all pixels to 'off'
      }
      
      void startAnimation() {
        mIsAnimating = true;
        mTurnLEDsOn = true;
        mNextLEDIndex = 0;
        mStartedWaiting = 0;
        if (mColor == 0) { // Currently displaying rainbow
          ++mRainbowShift %= NUM_LEDS;     // Shift the rainbow for the next step
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
            nextLEDColor = calculateColor(mNextLEDIndex + mRainbowShift, RAINBOW_EFFECT_FREQUENCY, 0);
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
  
        mStrip.setPixelColor((mNextLEDIndex + FIRST_LED) % NUM_LEDS, nextLEDColor);
        mStrip.show();
  
        mNextLEDIndex++; // Increment LED
        if (mNextLEDIndex >= NUM_LEDS) {
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

ShoeAnimation* animation = NULL;

class ColorButton : public Button {
  private:
    const int mNumColors;
    const float mFrequency;
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

ColorButton* colorButton = NULL;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(STEP_INPUT, INPUT_PULLUP);
  
  animation = new ShoeAnimation();
  colorButton = new ColorButton(COLOR_IN_PIN, COLOR_INPUT, 100, NUM_COLORS, COLOR_OFFSET, animation);
}
 
void loop() {
  // Check for color change
  colorButton->checkButtonState();
    
  if (animation->isAnimating()) { // Are we in the process of an LED animation?
    animation->incrementAnimation();
  } else if (checkIfStepping()) { // Take another step?
    animation->startAnimation();
  }
}

boolean checkIfStepping() {
  int stepSensorValue = analogRead(STEP_IN_PIN);

  #if TEST == 1 // print the results to the serial monitor:
  Serial.print("sensor = " );
  Serial.print(stepSensorValue);
  #endif
  
  return stepSensorValue < STEP_SENSOR_TRIGGER;
}

// Referenced from http://krazydad.com/tutorials/makecolors.php
uint32_t calculateColor(int i, float frequency, float offset) {
 return Adafruit_NeoPixel::Color(
      sin(frequency * (i + offset)) * 127 + 128,
      sin(frequency * (i + offset) + PI2_3) * 127 + 128,
      sin(frequency * (i + offset) + PI4_3) * 127 + 128
    ); 
}
