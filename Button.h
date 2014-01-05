#ifndef BUTTON_H
#define BUTTON_H

class Button {
  private:
    Button(const Button& copy); 
    Button& operator=(const Button& copy); 

    const int mPin;
    const int mThreshold;
    bool mIsDown;
      
  public:
    // Constructor: pin number, input number, threshold the value of the input must dip below to trigger a button push
    Button(int pin, int input, int threshold);
    
    // To be called in loop(). Checks if the button has been pushed or released.
    void checkButtonState();
    
    // Returns if the button is currently pushed down
    bool isDown() const;
    
  protected:    
    // Triggered when button is pushed down
    virtual void onDown() { }
    
    // Triggered when button is released
    virtual void onUp() { }
};
#endif

