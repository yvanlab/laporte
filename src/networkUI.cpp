
#include "main.h"

String getJson()
{
  String tt("{\"module\":{");
    tt += "\"nom\":\"" + String(MODULE_NAME) +"\"," ;
    tt += "\"version\":\"" + String(CURRENT_VERSION) +"\"," ;
    tt += "\"status\":\"" + String(STATUS_PERIPHERIC_WORKING) +"\"," ;
    tt += "\"uptime\":\"" + NTP.getUptimeString() +"\"," ;
    tt += "\"build_date\":\""+ String(__DATE__" " __TIME__)  +"\"},";
    tt += "\"setting\":{" + smManager.toString(JSON_TEXT)  + "},";
    tt += "\"LOG\":["+ wfManager.log(JSON_TEXT)  + "," +
                       dManager.log(JSON_TEXT) + ","+ fpManager.log(JSON_TEXT) + ","+ sManager.log(JSON_TEXT) + ","  +
                       smManager.log(JSON_TEXT)+"],";
    tt += dManager.toString(JSON_TEXT)+ ",";
    tt += "\"finger\":{" + fpManager.toString(JSON_TEXT)+ "},";
    tt += "\"datetime\":{" + wfManager.getHourManager()->toDTString(JSON_TEXT) + "}}";
    return tt;
}


void dataJson() {
	digitalWrite ( pinLed, LOW );
  wfManager.getServer()->send ( 200, "text/json", getJson() );
  digitalWrite ( pinLed, HIGH );
}

void setData(){
  //bool bWriteData = false;
  //String str;
  String str = wfManager.getServer()->arg("new");
  //String sId = wfManager.getServer()->arg("id");
  if (str.length()>0 && String("true").equalsIgnoreCase(str) ) {
    //uint8_t id = atoi(sId.c_str());
    fpManager.activateModule(true);
    delay(250);
    //startFingerprintActivated = millis();
    if (fpManager.recordFingerPrint(0, "" ) == FINGERPRINT_OK) {
      DEBUGLOGF("stored new finger [%d]\n ",fpManager.getTemplateCount());
      /*strcpy(smManager.m_name[id],name.c_str());
      smManager.writeData();*/
    } else {
      DEBUGLOG("stored new finger failure " + fpManager.log(STD_TEXT));
    }
  }
  str = wfManager.getServer()->arg("clean");
  if (String("true").equalsIgnoreCase(str)) {
    fpManager.emptyDatabase();
  }
  str = wfManager.getServer()->arg("open");
  if (String("true").equalsIgnoreCase(str)) {
    fpManager.activateModule(true);
    delay(250);
    dManager.activateModule(true);
  }
  dataPage();
}


void dataPage() {
	digitalWrite ( pinLed, LOW );

  String message =
  "<html>\
    <head>\
      <title>Summary page</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <h1>Real time data!</h1>";
  message += "<p>" + wfManager.toString(STD_TEXT) + "</p>";
  message += "<p>Date Hour : " + wfManager.getHourManager()->toDTString(STD_TEXT) + "</p>";
  message += "<p>Uptime: " + wfManager.getHourManager()->toUTString() + "</p>";
  message += "<p>&nbsp;</p>";
  message += "<p>Empreinte: " + String (fpManager.getTemplateCount()) + "/" + String (fpManager.getCapacityCount()) + "</p>";
  message += "<form method='get' action='setData'>";
  message += "<input type='checkbox' name='open' value='true'>Ouvrir la porte<br>";
  message += "<input type='checkbox' name='new' value='true'>Ajouter nouvelle empreinte<br>";
  message += "<input type='checkbox' name='clean' value='true'>Effacer toutes les empreintes<br>";
  /*message += "<TABLE border=2 cellpadding=10 log>";
  for (uint8_t i=0; i<4 ;i++) {
    message += "<TR><TD>"+String(i)+"</TD><TD>"+smManager.m_name[i]+"</TD></TR>";
  }
  message += "</TABLE>";
  message += "<form method='get' action='setData'>";
  message += "<label>Id:</label><input name='id' length=2 value=\"\">";
  message += "<label>Nome:</label><input name='name' length=20 value=\"\"><br>";
  message += "<label>open:</label><input name='open' length=2 value=\"\"><br>";
*/
  message += "<input type='submit'></form>";
  message += "<A HREF=\""+WiFi.localIP().toString()+ "\">cette page</A></br>";
  message += "<A HREF=\"https://thingspeak.com/channels/"+ String(smManager.m_privateKey) +"\">thingspeak</A></br>\
              <h2>Commands</h2>\
              <ul><li>/clear => erase credentials</li>\
                  <li>/credential\t=> display credential</li>\
                  <li>/restart\t=> restart module</li>\
                  <li>/status\t=> Json details</li>\
                  <li>/whatever\t=> display summary</li></ul>";
  message += "</body></html>";
  wfManager.getServer()->send ( 200, "text/html", message );

  digitalWrite ( pinLed, HIGH );

}
