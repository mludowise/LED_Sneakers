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
const float RAINBOW_EFFECT_FREQUENCY = 2 * PI / NUM_LEDS;  // Use this frequency when displaying rainbow effect. The shoe will show one rainbow cycle around its circumfrence.
const float SINGLE_COLOR_FREQUENCY = 2 * PI / NUM_COLORS;  // Use this frequency to calculate color of the LEDs when cycling through the colors.
const float PI2_3 = 2 * PI / 3;
const float PI4_3 = 4 * PI / 3;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_OUTPUT, NEO_GRB + NEO_KHZ800);
int rainbowShift = 0;                // this variable will shift the color that the rainbow effect starts with each time it's stepped on
int currentColor = 0;                // denotes the current color of the LEDs, cycled from 0 to NUM_COLORS
boolean colorButtonPressed = false;  // current state of the color button (true = pressed)
boolean isAnimating = false;         // is the shoe in the process of a swipe animation?
boolean turnLEDsOn = true;           // is the next step in the animation to turn the LEDs on or off?
int nextLED = 0;                     // the index of the next LED in the animation
unsigned long startedWaiting;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(STEP_INPUT, INPUT_PULLUP);
  pinMode(COLOR_INPUT, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}
 
void loop() {
  // Check for color change
  checkForColorChange();
  
  #if 0
  Serial.print("isAnimating = ");
  Serial.print(isAnimating);
  Serial.print("\tturnLEDsOn = ");
  Serial.print(turnLEDsOn);
  Serial.print("\tnextLED = ");
  Serial.print(nextLED);
  Serial.print("\tstartedWaiting = ");
  Serial.println(startedWaiting);
  #endif
  
  if (isAnimating) { // Are we in the process of an LED animation?
    Serial.print("Animating!");
    unsigned long now = millis();
    if (startedWaiting + WAIT <= now) { // If we've been waiting long enough for the next step in the animation
      changeNextLED();
      startedWaiting = now;
      Serial.print("\tWaited long enough");
    }
    Serial.println();
  } else if (checkIfStepping()) { // Take another step?
    Serial.println("Stepped!");
    isAnimating = true;
    turnLEDsOn = true;
    nextLED = 0;
    if (currentColor == NUM_COLORS) { // Currently displaying rainbow
      ++rainbowShift %= NUM_LEDS;     // Shift the rainbow for the next step
    }
    changeNextLED();
    startedWaiting = millis();
  }
}
  
void checkForColorChange() {
  int colorSensorValue = analogRead(COLOR_IN_PIN);
  
  #if TEST == 1
  Serial.print("\tbutton = ");
  Serial.println(colorSensorValue);
  #endif
  
  // Determine if color button has been pressed
  if (colorSensorValue < 100) { // Pressed
    colorButtonPressed = true;
    
    #if TEST == 1
    Serial.println("Button Pressed!");
    #endif
  } else if (colorButtonPressed) { // Depressed, change color
    colorButtonPressed = false;
    ++currentColor %= (NUM_COLORS + 1);
    
    #if TEST
    Serial.println("Button Released!");
    Serial.print("Color Index = ");
    Serial.println(currentColor);
    #endif
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

void changeNextLED() {
  uint32_t color = 0;
  if (turnLEDsOn) {
    Serial.print("turning on LED ");
    Serial.print(nextLED);
    if (currentColor == NUM_COLORS) { // Rainbow
      color = calculateColor(nextLED + rainbowShift, RAINBOW_EFFECT_FREQUENCY, 0);
      Serial.println(" to rainbow");
    } else { // Single color
      color = calculateColor(currentColor, SINGLE_COLOR_FREQUENCY, COLOR_OFFSET);
      Serial.println(" to color");
    }
  }
  
  #if TEST == 1
  Serial.print((color & 0xFF0000) >> 16);
  Serial.print("\t");
  Serial.print((color & 0x00FF00) >> 8);
  Serial.print("\t");
  Serial.println(color & 0x0000FF);
  #endif
  
  strip.setPixelColor((nextLED + FIRST_LED) % NUM_LEDS, color);
  strip.show();
  
  nextLED++; // Increment LED
  if (nextLED >= NUM_LEDS) {
    if (turnLEDsOn) {
      nextLED = 0;
      turnLEDsOn = false;
    } else {
      isAnimating = false;
    }
  }
}

/*
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor((i + FIRST_LED) % NUM_LEDS, c);
    strip.show();
    delay(wait);
  }
}

void rainbowWipe(int colorShift, uint8_t wait) {
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor((i + FIRST_LED) % NUM_LEDS, calculateColor(i + colorShift, RAINBOW_EFFECT_FREQUENCY, 0));
    strip.show();
    delay(wait);
  }
}
*/
// Referenced from http://krazydad.com/tutorials/makecolors.php
uint32_t calculateColor(int i, float frequency, float offset) {
 return strip.Color(
      sin(frequency * (i + offset)) * 127 + 128,
      sin(frequency * (i + offset) + PI2_3) * 127 + 128,
      sin(frequency * (i + offset) + PI4_3) * 127 + 128
    ); 
}
