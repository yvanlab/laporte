
#include "main.h"

/*
Organization ID :  wjv736
Device Type: ESP8266
Device ID : ESPDoorDebug
Authentication Method : use-token-auth
Authentication Token : jbHFqh(-hN2v?ZuTLS
*/


SettingManager      smManager(pinLed);
WifiManager         wfManager(pinLed,&smManager);
fingerprintManager  fpManager(pinLed);
grovestreamsManager     grovesMgt(pinLed);
DoorManager         dManager(pinLed);
SensorManager       sManager(pinLed);
FlashLed            flLed(pinLed);


//LedManager          ldManager(pinLed,BLOCKED_MODE);

#ifdef MCPOC_TELNET
RemoteDebug             Debug;
#endif

#ifdef MCPOC_TELNET // Not in PRODUCTION
void processCmdRemoteDebug() {
    String lastCmd = Debug.getLastCommand();
    if (lastCmd == "o") {
      dManager.activateModule(true);
    } else if (lastCmd == "r") {
      fpManager.recordFingerPrint(0,"yvan");
    } else if (lastCmd == "a") {
      fpManager.activateModule(true);
    } else if (lastCmd == "restart") {
        ESP.restart();
    } else if (lastCmd == "reset") {
      WiFi.disconnect();
    }
}
#endif


void startWiFiserver() {
  if (wfManager.begin(IPAddress(MODULE_IP),MODULE_NAME, MODULE_MDNS, MODULE_MDNS_AP)==WL_CONNECTED) {
    wfManager.getServer()->on ( "/", dataJson );
    wfManager.getServer()->onNotFound ( dataJson );
  }
  wfManager.getServer()->on ( "/status", dataJson );
  wfManager.getServer()->on ( "/setting", claraPage );
    wfManager.getServer()->on ( "/s", dataPage );
  wfManager.getServer()->on ( "/setData", setData );


  Serial.println(wfManager.toString(STD_TEXT));
}

void setup ( void ) {
  ESP.wdtDisable() ;
  Serial.begin ( 115200 );
  delay(500);
  Serial.println ( "start debuging" );
  delay(5000);
  smManager.readData();
  DEBUGLOG("");DEBUGLOG(smManager.toString(STD_TEXT));
  startWiFiserver();

  #ifdef MCPOC_TELNET
    Debug.begin(MODULE_NAME);
    String helpCmd = "l1\n\rl2\n\rl3\n\rrestart\n\rreset\n\r";
    Debug.setHelpProjectsCmds(helpCmd);
    Debug.setCallBackProjectCmds(&processCmdRemoteDebug);
  #endif

  #ifdef MCPOC_MOCK
  randomSeed(analogRead(0));
  #endif

  mtTimer.onTimerAction(std::bind(&FlashLed::flashCallback,&flLed));
  mtTimer.begin(timerFrequence);
  mtTimer.setCustomMS(3000); //10s

  #ifdef MCPOC_TEST
  Serial.println("Testing device connections...");
  #endif

  fpManager.begin();
  dManager.begin();

  //ldManager.begin();

  //attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);


}

DelayHelper waitForTime;
void loop ( void ) {

	wfManager.handleClient();
  fpManager.handle();
  dManager.handle();
  sManager.handle();
  //ldManager.handle();

  if (sManager.isActivated()) {
    //ldManager.activateModule(true);
    if (waitForTime.isDone()) {
      waitForTime.startDelay(30000);
      smManager.nbCouloirDetection++;
    }
    if (!fpManager.isActivated())
      flLed.startFlashLed(25,FLASH_ON);
    fpManager.activateModule(true);
  }

  if (fpManager.isActivated()) {
    if (fpManager.readFingerPrint() == FINGERPRINT_OK) {
      //fpManager.activateModule(false);
      dManager.activateModule(true);
      smManager.nbDressingDoor++;
    }
  }


  if (mtTimer.is5MNPeriod()) {
    if (!WiFi.isConnected()) {
      ESP.restart();
    }
  }



  if (mtTimer.is1HPeriod()) {
    if (WiFi.isConnected()) {
      grovesMgt.addVariable(PRESENCE_COULOIR   , String(smManager.nbDressingDoor));
      grovesMgt.addVariable(PRESENCE_DRESSING  , String(smManager.nbCouloirDetection));
      grovesMgt.sendIoT(PRESENCE_ID);
      smManager.nbCouloirDetection = 0;
      smManager.nbDressingDoor = 0;
    }
  }
  mtTimer.clearPeriod();
}
