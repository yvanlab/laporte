

#ifndef LedManager_h
#define LedManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include "DurationManager.h"

#define MY_DOOR_PIN D0

class LedManager : public DurationManager
{
  public:
    LedManager(uint8_t pinLed, bool bBlockedMode) : DurationManager(pinLed, bBlockedMode) {
      activateModule(false);
      setActivationDuration(25);
    } ;

    /*uint8_t begin() {
      setActivationDuration(25);
      return 0;
    };*/

    void myactivate() {
      DEBUGLOG("Switch on led");
      switchOn();

    }
    void mydeactivate() {
      DEBUGLOG("Switch off led");
      switchOff();
    }


      virtual String getClassName(){ return "led";}


  private:


};

#endif
