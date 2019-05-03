
#ifndef main_h
#define main_h

#include <Arduino.h>


#include <WifiManagerV2.h>
#include <myTimer.h>
#include <flashLed.h>
#include <DelayHelper.h>

#include "SettingManager.h"
#include "fingerprintManager.h"
#include "doorManager.h"
#include "sensorManager.h"
#include "networkUI.h"
#include "grovestreamsManager.h"

//#include "ledManager.h"





#define MODULE_NAME LAPORTE_NAME
#define MODULE_MDNS LAPORTE_MDNS
#define MODULE_MDNS_AP LAPORTE_MDNS_AP
#define MODULE_IP   LAPORTE_IP



#define LAPORTE_OUVERTURE_LABEL  1 //"current"

int getFingerprintEnroll(int id);
bool get_free_id(int16_t * id);
void dataPage();


#define  pinLed  D4


extern SettingManager      smManager;
extern WifiManager         wfManager;
extern fingerprintManager  fpManager;
extern DoorManager         dManager;
extern SensorManager       sManager;
extern FlashLed            flLed;
extern grovestreamsManager grovesMgt;

#ifdef MCPOC_TELNET
//extern RemoteDebug          Debug;
#endif



#endif
