
#ifndef DurationManager_h
#define DurationManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <BaseManager.h>

#define DUR_UNLIMITED 0

#define UNBLOCKED_MODE false
#define BLOCKED_MODE true

class DurationManager : public BaseManager
{
  public:
    DurationManager(uint8_t pinLed, bool bBlockedMode = UNBLOCKED_MODE) : BaseManager (pinLed){
      m_duration = DUR_UNLIMITED;
      m_bBlockedMode = bBlockedMode;
      m_bDone = false;
    } ;
    //~DurationManager();

    void activateModule(bool activate) {
      if (m_activated && activate) return;
      m_activated = activate;
      if (activate && !(m_bBlockedMode && m_bDone)) {
        myactivate();
        m_startActivated = millis();
        m_bDone = false;
      }
      else {
        mydeactivate();
        m_startActivated = 0;
        m_bDone = true;
      }
    }
    void setActivationDuration(uint16_t  duration) {
      m_duration = duration;
      m_bDone = false;
    }

    void reset() {
      m_bDone = false;
    }

    virtual void handle() {
      if (m_duration == DUR_UNLIMITED || !m_activated ) return;
      if ((millis() - m_startActivated) > m_duration) {
        activateModule(false);
      }

    }

    bool isActivated() {return m_activated;}
    virtual void myactivate()=0;
    virtual void mydeactivate()=0;


  protected:
    bool            m_activated;
    bool            m_bBlockedMode;
    bool            m_bDone;
    long            m_startActivated;
    uint16_t        m_duration;



};

#endif
