#include "Button.h"
#include "ShoeAnimation.h"
#include "ColorButton.h"
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define PRINT_SENSOR_VALUE true    // Print the step sensor value to the console

// Define all our inputs and outputs
#define STEP_IN_PIN A9              // Analog input pin that the step potentiometer is attached to
#define STEP_INPUT 9                // Analog input pin that the step potentiometer is attached to
#define COLOR_IN_PIN A10            // Analog input pin that the color potentiometer is attached to
#define COLOR_INPUT 10              // Analog input pin that the color potentiometer is attached to
#define LED_OUTPUT 6                // Output pin that the Neo Pixel LEDs are attached to

// Set these based on individual shoe and potentiometer sensitivies
// Right = 44 - 100 (75)
// Left = 50 - 115
#define STEP_SENSOR_TRIGGER 75      // The LEDs will be triggered when the step input goes below this value
#define NUM_LEDS 38                 // The number of LEDs on the shoe
#define FIRST_LED 34                // The index of the first LED on the shoe to start the cycle with
#define REVERSE_ANIMATION true      // If the LED animation should animate from outside-in

// Some constants used for display
#define NUM_COLORS 8                // The number of colors to cycle through with button clicks
#define COLOR_OFFSET 0.8f           // Value between 0 and 1, used to tweak the exact colors that are cycled through
#define ANIMATION_DELAY 25          // Time delay to wait for LEDs to light up

ShoeAnimation* animation = NULL;
ColorButton* colorButton = NULL;
Button* stepButton = NULL;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  animation = new ShoeAnimation(LED_OUTPUT, NUM_LEDS, FIRST_LED, ANIMATION_DELAY, REVERSE_ANIMATION);
  colorButton = new ColorButton(COLOR_IN_PIN, COLOR_INPUT, 100, NUM_COLORS, COLOR_OFFSET, animation, 0);
  stepButton = new Button(STEP_IN_PIN, STEP_INPUT, STEP_SENSOR_TRIGGER);
  stepButton->printSensorValue(PRINT_SENSOR_VALUE);
}
 
void loop() {
  // Check for inputs
  colorButton->checkButtonState();
  stepButton->checkButtonState();
    
  if (animation->isAnimating()) {    // Are we in the process of an LED animation?
    animation->increment();          // Try to increment the animation
  } else if (stepButton->isDown()) { // Is someone stepping?
    animation->start();              // Start animation
  }
}
