/*
Switch
Copyright (C) 2012  Albert van Dalen http://www.avdweb.nl
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License at http://www.gnu.org/licenses .

Version 20-4-2013
_debounceDelay=50
Version 22-5-2013
Added longPress, doubleClick

                        _______________________      _false
                       |                       |   || |
 input                 |                       || ||| |
                  _____|                       ||_||| ||____________

 poll                   ^                        ^   ^          ^
 switchedTime           ^                        ^
                         <---------100ms--------><--->
 debounceDelay           <----50ms---->          <----50ms---->
 switched               1                        1   0          0
 newlevel               1                        0   1          0
                         ________________________
 level             _____|                        |___________________

.......................................................................
                            _______________________             _______
                           |                       |           |
 input                     |                       |           |
                   ________|                       |___________|

 longPressDelay             <----------->

 doubleClickDelay           <-------------------------------------->
                                          _________
 _longPressLatch   ______________________|         |_________________
                                          _
 _longPress        ______________________| |__________________________
                                                                _
_doubleClick       ____________________________________________| |____

*/

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include "Switch.h"

// level(0)
Switch::Switch(byte _pin, byte PinMode, bool polarity, int debounceDelay, int longPressDelay, int doubleClickDelay):
pin(_pin), debounceDelay(debounceDelay), longPressDelay(longPressDelay), doubleClickDelay(doubleClickDelay), polarity(polarity)
{ pinMode(pin, PinMode);
  _switchedTime = millis();
  level = digitalRead(pin);
}

bool Switch::poll()
{ _longPress = _doubleClick = false;
  bool newlevel = digitalRead(pin);

  if(!_longPressLatch)
  { _longPress = on() && ((millis() - pushedTime) > longPressDelay); // true just one time between polls
    _longPressLatch = _longPress; // will be reset at next switch
  }
  if(_longPressCallback && longPress())
  { _longPressCallback(_longPressCallbackParam);
  }

  if((newlevel != level) & (millis() - _switchedTime >= debounceDelay))
  { _switchedTime = millis();
    level = newlevel;
    _switched = 1;
    _longPressLatch = false;

    if(pushed())
    { _doubleClick = (millis() - pushedTime) < doubleClickDelay;
      pushedTime = millis();
    }

    if(_pushedCallback && pushed())
    { _pushedCallback(_pushedCallbackParam);
	}
	else if(_releasedCallback && released())
	{ _releasedCallback(_releasedCallbackParam);
	}

    if(_doubleClickCallback && doubleClick())
    { _doubleClickCallback(_doubleClickCallbackParam);
	}
    return _switched;
  }
  return _switched = 0;
}

bool Switch::switched()
{ return _switched;
}

bool Switch::on()
{ return !(level^polarity);
}

bool Switch::pushed()
{ return _switched && !(level^polarity);
}

bool Switch::released()
{ return _switched && (level^polarity);
}

bool Switch::longPress()
{ return _longPress;
}

bool Switch::doubleClick()
{ return _doubleClick;
}

bool Switch::longPressLatch()
{ return _longPressLatch;
}

void Switch::setPushedCallback(switchCallback_t cb, void* param)
{ /// Store the "pushed" callback function
  _pushedCallback = cb;
  _pushedCallbackParam = param;
}

void Switch::setReleasedCallback(switchCallback_t cb, void* param)
{ /// Store the "released" callback function
  _releasedCallback = cb;
  _releasedCallbackParam = param;
}

void Switch::setLongPressCallback(switchCallback_t cb, void* param)
{ /// Store the "long press" callback function
  _longPressCallback = cb;
  _longPressCallbackParam = param;
}

void Switch::setDoubleClickCallback(switchCallback_t cb, void* param)
{ /// Store the "double click" callback function
  _doubleClickCallback = cb;
  _doubleClickCallbackParam = param;
}

unsigned long Switch::pushedDuration() {
  if (on()) {
    unsigned long dur = millis() - pushedTime;
    if (!dur) {
      dur = 1;  // minimum 1ms
    }
    return dur;
  } else {
    return 0;
  }
}
