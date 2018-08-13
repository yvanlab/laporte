

#ifndef DoortManager_h
#define DoortManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <SoftwareSerial.h>
#include <FPM.h>
#include "SettingManager.h"
#include "DurationManager.h"

#define MY_DOOR_PIN D0

class DoorManager : public DurationManager
{
  public:
    DoorManager(uint8_t pinLed) : DurationManager(pinLed) {
      activateModule(false);
    } ;

    uint8_t begin() {
      setActivationDuration(5000);
      return 0;
    };

    void myactivate() {
      DEBUGLOG("Open the door");
      pinMode ( MY_DOOR_PIN, OUTPUT );

    }
    void mydeactivate() {
      DEBUGLOG("close the door");
      pinMode ( MY_DOOR_PIN, INPUT );
      setActivationDuration(5000);
    }

    String toString(boolean bJson) {
      if (bJson)
          return "\"open_door\" : \""+String (isActivated()) + "\"";
      else
          return "open_door ["+String (isActivated()) + "]";
      }
      
      virtual String getClassName(){ return "door";}


  private:


};

#endif
