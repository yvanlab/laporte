#include "fingerprintManager.h"
// pin #D2 is IN from sensor (GREEN wire)
// pin #D3 is OUT from arduino  (WHITE/YELLOW wire) TX
#define RX  D1 //green
#define TX  D2 //(blanc)
#include <myTimer.h>
#include "FlashLed.h"

extern MyTimer         mtTimer;
extern FlashLed        flLed;

fingerprintManager::fingerprintManager(uint8_t pinLed): DurationManager(pinLed){
  pinMode ( MY_FINGERPRINT_PIN, OUTPUT);
  activateModule(true);
}

fingerprintManager::~fingerprintManager(){
  //if (m_fingerSerial) delete (m_fingerSerial);
};

uint8_t fingerprintManager::begin() {
  DEBUGLOG("fingertest");
  m_fingerSerial = new SoftwareSerial(RX, TX);
  m_fingerSerial->begin(57600);

  if (m_finger.begin(m_fingerSerial)) {
    DEBUGLOG("Found fingerprint sensor!");
    m_finger.getTemplateCount();
    DEBUGLOGF("Capacity[%d], Packet length[%d], Template[%d]\n", m_finger.capacity,m_finger.packetLen, m_finger.templateCount);
    setStatus(FINGERPRINT_OK,getStatusString(FINGERPRINT_OK));
  } else {
    DEBUGLOG("Did not find fingerprint sensor :-(");
    setStatus(FINGERPRINT_NOTFOUND,getStatusString(FINGERPRINT_NOTFOUND));
  }
  setActivationDuration(5000);
  return getStatusCode();
}

uint8_t fingerprintManager::emptyDatabase() {
  uint8_t res = m_finger.emptyDatabase();
  setStatus(res,getStatusString(res));
  DEBUGLOGF("empty DB %d,%s\n",res,getStatusString(res).c_str());
  m_finger.getTemplateCount();
  return res;
}

int16_t fingerprintManager::get_free_id(){
  int16_t p = -1;
  int16_t id;
  for (int page = 0; page < (m_finger.capacity / FPM_TEMPLATES_PER_PAGE) + 1; page++){
    p = m_finger.getFreeIndex(page, &id);
    if (p == FINGERPRINT_OK) {
      if (id != FINGERPRINT_NOFREEINDEX){
        DEBUGLOGF("Free ID %d\n",id);
        setStatus(p,getStatusString(p));
        return id;
      }
    } else {
      setStatus(p,getStatusString(p));
      DEBUGLOGF("Free ID Error  %d,%s\n",p,getStatusString(p).c_str());
      return -1;
    }
    yield();
  }
  setStatus(FINGERPRINT_NOFREEINDEX,getStatusString(FINGERPRINT_NOFREEINDEX));
  return -2;
}

String  fingerprintManager::getStatusString(int16_t p){
  switch (p) {
    case FINGERPRINT_OK:
      return "OK";
    case FINGERPRINT_NOFINGER:
      return "No finger";
    case FINGERPRINT_PACKETRECIEVEERR:
      return "Communication error";
    case FINGERPRINT_IMAGEFAIL:
      return "Imaging error";
    case FINGERPRINT_FEATUREFAIL:
      return "Could not find fingerprint features";
    case FINGERPRINT_INVALIDIMAGE:
      return "Invalid image";
    case FINGERPRINT_NOTFOUND:
      return "Did not find a match";
    case FINGERPRINT_ENROLLMISMATCH:
      return "Fingerprints did not match";
    case FINGERPRINT_BADLOCATION:
       return "Could not store in that location";
    case FINGERPRINT_FLASHERR:
       return "Error writing to flash";
    case FINGERPRINT_NOFREEINDEX:
        return "No free index";
    case FINGERPRINT_BADPACKET:
        return "Bad packet";
    default:
      return "Unknown error";
  }

}

uint8_t fingerprintManager::readFingerPrint() {
  uint8_t p = FINGERPRINT_NOFINGER;
  DEBUGLOG("Waiting for a finger...");

  //while (p != FINGERPRINT_OK){
  p = m_finger.getImage();
  setStatus(p,getStatusString(p));
  DEBUGLOGF("reading %d,%s\n",p,getStatusString(p).c_str());
  if (p != FINGERPRINT_OK) return p;
  /*  yield();
}*/
  switchOn();
  // OK success!

  p = m_finger.image2Tz();
  setStatus(p,getStatusString(p));
  DEBUGLOGF("Convert %d,%s\n",p,getStatusString(p).c_str());
  if (p != FINGERPRINT_OK) return stopFailed(p);
  // OK converted!stopFailed(p)
  p = m_finger.fingerFastSearch();
  setStatus(p,getStatusString(p));
  DEBUGLOGF("Search %d,%s\n",p,getStatusString(p).c_str());
  if (p != FINGERPRINT_OK)  {
     return stopFailed(p);
   }
  // found a match!
  DEBUGLOGF("Found ID %d/%d\n",m_finger.fingerID,m_finger.confidence);
  return stopSuccess( FINGERPRINT_OK );
}


uint8_t fingerprintManager::recordFingerPrint(uint8_t id, String name){

  uint8_t p = FINGERPRINT_NOFINGER;
  DEBUGLOGF("Waiting for valid finger to enroll");
  mtTimer.initTimeOut();
  while (p != FINGERPRINT_OK  ) {
    p = m_finger.getImage();
    setStatus(p,getStatusString(p));
    DEBUGLOGF("reading %d,%s\n",p,getStatusString(p).c_str());
    if (mtTimer.is25MSFrequence())
      switchOn();
    else
      switchOff();
    yield();
    if (mtTimer.isTimeOut(MY_FINGERPRINT_TIMEOUT)) {
      return stopFailed( FINGERPRINT_TIMEOUT );
    }
  }
  // OK success!


  p = m_finger.image2Tz(1);
  setStatus(p,getStatusString(p));
  DEBUGLOGF("Convert1 %d,%s\n",p,getStatusString(p).c_str());
  if (p != FINGERPRINT_OK) return stopFailed(p);

  /*Serial.println("Remove finger");*/
  //delay(2000);

  mtTimer.initTimeOut();
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = m_finger.getImage();
    if (mtTimer.is250MSFrequence())
      switchOn();
    else
      switchOff();
    yield();
    if (mtTimer.isTimeOut(MY_FINGERPRINT_TIMEOUT)) {
      return stopFailed( FINGERPRINT_TIMEOUT );
    }
  }

  p = FINGERPRINT_NOFINGER;
  Serial.println("Place same finger again");
  mtTimer.initTimeOut();
  while (p != FINGERPRINT_OK) {
    p = m_finger.getImage();
    setStatus(p,getStatusString(p));
    DEBUGLOGF("reading %d,%s\n",p,getStatusString(p).c_str());
    if (mtTimer.is25MSFrequence())
      switchOn();
    else
      switchOff();
    yield();
    if (mtTimer.isTimeOut(MY_FINGERPRINT_TIMEOUT)) {
      return stopFailed( FINGERPRINT_TIMEOUT );
    }
  }
  switchOn();
  p = m_finger.image2Tz(2);
  DEBUGLOGF("Convert2 %d,%s\n",p,getStatusString(p).c_str());
  if (p != FINGERPRINT_OK) return stopFailed(p);


  // OK converted!
  p = m_finger.createModel();
  DEBUGLOGF("Convert2 %d,%s\n",p,getStatusString(p).c_str());
  if (p != FINGERPRINT_OK) return stopFailed(p);
  int16_t _id = get_free_id();

  p = m_finger.storeModel(_id);
  setStatus(p,getStatusString(p));
  DEBUGLOGF("Store ID[%d], %d,%s\n",id,p,getStatusString(p).c_str());
  m_finger.getTemplateCount();
  switchOff();
  return getStatusCode();
}

uint8_t fingerprintManager::stopFailed(uint8_t p) {
  setStatus(p,getStatusString(p));
  switchOff();
  return p;
}

uint8_t fingerprintManager::stopSuccess(uint8_t p) {
  setStatus(p,getStatusString(p));
  flLed.startFlashLed(1000,FLASH_25ms);
  /*mtTimer.initTimeOut();
  while (!mtTimer.isTimeOut(1000)) {
    if (mtTimer.is25MSFrequence())
      switchOn();
    else
      switchOff();
    yield();
  }
  switchOff();*/
  return p;
}
