

#include "SettingManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

SettingManager::SettingManager(unsigned char pinLed) : BaseSettingManager(pinLed){

}

unsigned char SettingManager::readData(){
  BaseSettingManager::readData();
  switchOn();
  for (uint8_t i =0;i<4;i++ )
    readEEPROM(m_name[i],10);
  switchOff();
}
unsigned char SettingManager::writeData(){
  BaseSettingManager::writeData();
  switchOn();
  for (uint8_t i =0;i<4;i++ )
    writeEEPROM(m_name[i]);
  EEPROM.commit();
  switchOff();
}

String SettingManager:: toString(boolean bJson = false){
  String ss;
  if (bJson ==STD_TEXT) {
    ss = BaseSettingManager::toString(bJson);
    for (uint8_t i =0;i<4;i++ )
      ss = ss + " name_"+i+"["+ m_name[i] +"]";
  } else {
    for (uint8_t i =0;i<3;i++ )
      ss = ss + "\"name_"+i+ "\":\""+ m_name[i] + "\"," ;
    ss = ss + "\"name_3\":\""+ m_name[3]+"\"" ;
  }
  return ss;
}
