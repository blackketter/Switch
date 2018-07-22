#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include "Switch.h"

const byte toggleSwitchpin = 3; // (right button)
const byte buttonGNDpin = 4; // (left button)
const byte ButtonVCCpin = 6;
const byte Button10mspin = 8;

Switch buttonGND = Switch(buttonGNDpin); // button to GND, use internal 20K pullup resistor
Switch toggleSwitch = Switch(toggleSwitchpin);
Switch buttonVCC = Switch(ButtonVCCpin, INPUT, HIGH); // button to VCC, 10k pull-down resistor, no internal pull-up resistor, HIGH polarity
Switch button10ms = Switch(Button10mspin, INPUT_PULLUP, LOW, 1); // debounceTime 1ms

void buttonCallbackFunction(void* s) {
  Serial.print("Button: ");
  Serial.println((char*)s);
}

void toggleCallbackFunction(void* s) {
  Serial.print("Toggle: ");
  Serial.println((char*)s);
}

void setup()
{ 
  Serial.begin(9600);
  buttonGND.setPushedCallback(&buttonCallbackFunction, (void*)"pushed");
  buttonGND.setReleasedCallback(&buttonCallbackFunction, (void*)"released");

  toggleSwitch.setLongPressCallback(&toggleCallbackFunction, (void*)"long press");
  toggleSwitch.setDoubleClickCallback(&toggleCallbackFunction, (void*)"double click");
}

void loop()
{ 
  buttonGND.poll();
  toggleSwitch.poll();
}
