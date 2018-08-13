

#ifndef fingerprintManager_h
#define fingerprintManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <SoftwareSerial.h>
#include <FPM.h>
#include "SettingManager.h"
#include "DurationManager.h"

#define MY_FINGERPRINT_TIMEOUT 30000
#define MY_FINGERPRINT_PIN D7

class fingerprintManager : public DurationManager
{
  public:
    fingerprintManager(uint8_t pinLed);
    ~fingerprintManager();
    uint8_t begin();
    uint8_t readFingerPrint();
    uint8_t recordFingerPrint(uint8_t id, String name);
    uint8_t emptyDatabase();
    int16_t  get_free_id();
    uint16_t getTemplateCount() {
      return m_finger.templateCount;
      /*uint8_t res = m_finger.getTemplateCount();
      if ( res == FINGERPRINT_OK){
        return m_finger.templateCount;
      } else {
        setStatus(res,getStatusString(res));
        return 0;
      }*/
    };


    uint16_t getCapacityCount() {
      return m_finger.capacity;
    }


    void myactivate() {
      digitalWrite(MY_FINGERPRINT_PIN, LOW);
    }
    void mydeactivate() {
      digitalWrite(MY_FINGERPRINT_PIN, HIGH);
    }

  String toString(boolean bJson) {

      if (bJson)
          return  "\"active\" : \""+String (isActivated()) + "\"," +
                  "\"empreinte\" : \""+String (getTemplateCount()) + "\"," +
                  "\"capacity\" : \""+String (m_finger.capacity) + "\"," +
                  "\"packet\" : \""+String (m_finger.packetLen) + "\"";
      else
          return "finger ["+String (isActivated()) + "] nb template ["+String (getTemplateCount())+"]";
      }

      virtual String getClassName(){ return "fingerprint";}

  private:
    SettingManager *m_stManager;
    SoftwareSerial *m_fingerSerial;
    FPM             m_finger;


    String getStatusString(int16_t p);
    uint8_t stopFailed(uint8_t p);
    uint8_t stopSuccess(uint8_t p);



};

#endif
