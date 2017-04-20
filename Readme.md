# Switch library

Copyright (C) 2012  Albert van Dalen <http://www.avdweb.nl>

For more information see: <http://www.avdweb.nl/arduino/hardware-interfacing/simple-switch-debouncer.html>

### Arduino library for debouncing switches and buttons

Switches and push buttons need debouncing. It is straightforward to do this with software, using a library. The advantages of the Switch library are:

- External pull-up resistors are not required.
- Supports also long press and double clicks.
- Performs not just de-bouncing, but also de-glitching against EMC pulses.

#### New: software deglitch

Normally, just debouncing is used. But debouncing doesn't make the software insensitive to noise pulses, which could lead to dangerous situations: if there is a poll() at the moment of a noise pulse, the software can detect an activated switch. To prevent this, a deglitch function is added now: during a "deglitchPeriod", all input values have to be equal.

#### Connecting switches to the Arduino

There is nothing needed beyond the switches, just connect the switches between the ground and a digital pin:

#### Switch between GND and digital pin

This is technically speaking the best solution and is taken by the library as default. An external pull-up resistor is not needed but allowed.

#### Switch between 5V and digital pin

For switches connected to the Arduino power supply, the settings are: polarity = HIGH and pinmode = INPUT, which disables the internal pull-up resistor. Note that we need external pull-down resistors of about 10k here.

#### Using the Switch library

All switched have to be polled individually to update the status. Each switch-status has its own get function:

#### pushed()

Use only for push buttons. It returns "true" if a button was pushed after the poll() instruction was executed. Note that this is in fact "single-click".

#### released()

It returns "true" if a push button was released, this will however rarely be used.

#### on()

Use only for toggle switches. It returns "true" as long as the switch is in the "on" position. The polarity of the switch in the "on" position has to be filled in correctly. There is no off() function, this is simply !on().

#### longPress()

It returns "true" if a push button is pressed longer than 300ms. The note at doubleClick() applies also here.

#### doubleClick()

It returns "true" if a push button is double clicked within 250ms. Note that a doubleClick() always will be preceded by pushed() from the first push. This can't be avoided, because then the pushed() function would have to wait on a possible second click, which would introduce an annoying delay. So, the action on doubleClick() has to undone the previous action on pushed().

### Notes

The poll instruction must be called at least 50 times per second, it takes about 20us.

The button states are saved till the next poll. Then all previous button states will be cleared, so the buttons must be read every poll interval.

Reading a button several times within a poll interval gives the same value, so reading doesn't clear the button state.
How to use successive button events

With the Switch library, you can use all button events at the same time with the same button. For example pushed(), released(), doubleClick() and longPress(). To see how several button events can be used together, run Windows Explorer: a single click selects a file and a double click opens it.

#### Button pushed event followed by a longPress event

A long-press generates first a pushed event and after 300ms the longPress event. This is not a shortcoming but a logical consequence. We can, of course, not always wait 300ms to see if a push might be a long push.

#### Button pushed event followed by a doubleClick event

The same happens with doubleClick, which also generates two pushed() events. When doubleClick is used, ignore the second pushed() result or don't call pushed(). When doubleClick is not needed, simply don't call doubleClick().

#### Software debounce, how does it work

The software debounce algorithm is based on the following assumptions; the 50ms is the so-called debounce delay:

The switch bounce time is less than 50ms.
The time between successive keystrokes is larger than 50ms.
A reaction time of 50ms is acceptable.
If the above assumptions are met, the software debounce algorithm can be quite simple: a switch event is only accepted when the elapsed time since the last switch event is larger than the debounce delay.

A debounce delay of 50ms is a safe value; it doesn't hurt the reaction time, and will handle even bad switches. 

```C++
if((newlevel != level) & (millis() - _switchedTime >= debounceDelay))
```

#### Status callbacks 

Callbacks can be used, instead of needing to call `switch.poll()` and then checking with an `if` statement whether the relevant event has occurred. 
With a callback registered, Switch will run the defined function when the conditions are met. 

Define a function you want to perform, with no return type and no arguments: 


```C++
void foo() {
	//your code;
}
``` 
and then call one of the four "set callback" methods in the Arduino setup function, like so: 

```C++
void setup() {
	... // Other setup code 
	... 
	switch.setPushedCallback(&foo);
	...
}
``` 

There is also `setReleasedCallback`, `setLongPressCallback`, and `setDoubleClickCallback`. If using a toggle switch and not a push button, the "pressed" event will be of interest when the switch is turned on, and "released" when it is turned off. 


#### Using an interrupt service routine for polling the buttons

Polling buttons has a high priority, slow functions such as Serial.print() may disrupt the timing. See here how to use an ISR for polling the buttons:

```C++
#include <Arduino.h>
#include "Switch.h"
#include <FrequencyTimer2.h>
  
Switch speedUpBtn(1);
Switch speedDownBtn(2);
Switch buttonLeft(3);
Switch buttonRight(4);
  
void setup(void)
{ Serial.begin(9600);
  FrequencyTimer2::setPeriod(1000);
  FrequencyTimer2::setOnOverflow(timer2ISR);
}
  
void loop(void)
{ printAll(); // run slow functions in loop()
}
  
void timer2ISR()
{ pollAll(); // polling buttons has priority
  buttonActions();
}
```

See also: [A Guide To Debouncing](http://www.eng.utah.edu/~cs5780/debouncing.pdf)
