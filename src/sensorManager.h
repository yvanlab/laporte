
#ifndef SensorManager_h
#define SensorManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <durationManager.h>

#define INPUT_SENSOR_PIN D6

class SensorManager : public DurationManager
{
  public:
    SensorManager(uint8_t pinLed) : DurationManager (pinLed){
        pinMode(INPUT_SENSOR_PIN,INPUT_PULLDOWN_16);
        setActivationDuration(250);
    } ;
    //~DurationManager();

    void handle() {
      //DEBUGLOG(digitalRead(INPUT_SENSOR_PIN));
      if (m_activated && ((millis() - m_startActivated) < m_duration))
        return ;
      activateModule(digitalRead(INPUT_SENSOR_PIN));
    }

    virtual void myactivate(){
      m_activated = true;
    };

    virtual void mydeactivate(){
      m_activated = false;
    };
    virtual String getClassName(){ return "Sensor";}

};

#endif
