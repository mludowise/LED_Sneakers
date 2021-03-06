#ifndef SHOE_ANIMTATION_H
#define SHOE_ANIMTATION_H

#include <Adafruit_NeoPixel.h>
#include "Arduino.h"

class ShoeAnimation {
  private:
    ShoeAnimation(const ShoeAnimation& copy); 
    ShoeAnimation& operator=(const ShoeAnimation& copy); 
    
    const int NUM_LEDS;                  // The number of LEDs on the shoe
    const int FIRST_LED_INDEX;           // The index of the first LED on the shoe to start the cycle with
    const float RAINBOW_COLOR_FREQUENCY; // Use this frequency when displaying rainbow effect. The shoe will show one rainbow cycle around its circumfrence.
    const int DELAY;                     // The amount of time, in milliseconds, to delay between pixel animations
    
    Adafruit_NeoPixel mStrip;            // The NeoPixel LED strip
    bool mIsAnimating;                   // Is the shoe in the process of a swipe animation?
    bool mTurnLEDsOn;                    // Should the next step in the animation to turn the LEDs on or off?
    int mNextLEDIndex;                   // The index of the next LED in the animation
    unsigned long mStartedWaiting;       // Time we started waiting for the next step of animation
    int mRainbowOffset;                  // The color of the rainbow is offset by one LED with each step
    uint32_t mColor;                     // Color the LEDs will display, 0 indicates rainbow mode
    bool mReverseAnimation;              // If true, the direction of the animation goes backwards (mNextLEDIndex will be decremented rather than incremented)
    
    // Sets the LED with the specified index to the specified color, offsetting it by FIRST_LED_INDEX (color=0 sets the LED off)
    void setPixelColor(int index, uint32_t color);
    
    // Returns the color for the LED with the specified index. 
    // Returns a different color for each index if in rainbow mode, otherwise returns mColor;
    uint32_t getLEDColor(int index);

  public:
    
    // Constructor: output pin, number of LEDs, index of the first LED, time delay in milliseconds, direction of the animation
    ShoeAnimation(int output, int numLEDs, int firstLEDIndex, int delayMillis, bool reverseAnimation);
    
    // Starts a new animation
    void start();
    
    // Increments the animation by one LED
    void increment();
    
    // Returns if an animation is in process
    bool isAnimating() const;
    
    // Sets the color of the LEDs, assumes rainbow mode if c == 0
    void setColor(uint32_t c);
};

#endif
