#ifndef DATA_INCLUDE_H
#define DATA_INCLUDE_H
//test
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_user = "";    
const char* mqtt_pass = "";    
const char* topic_pub = "test001"; 
//endtest

WiFiClient espClient;
PubSubClient client(espClient);
Preferences preferences;

struct wifi_info {
    String EEPROM_SSID ;
    String EEPROM_Pass ;
    String deviceId ;

    String apSsid ;
    String apPassword ;
    String apIP ;
    String apHostname ;

    String IP ;
    String mac ;
    String subnet;
    String gateway ;
    String StartTime ;
};
wifi_info wifiInfo;

struct tm timeinfo;

String gettime() {
  if (getLocalTime(&timeinfo)) {
    char timeString[50];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);

    Serial.println("Synced time: " + String(timeString));
    return String(timeString);
  } else {
    Serial.println("Failed to obtain time");
    return "";
  }
}

String getEEprom(String name , String key = ""){
  String res ;
  preferences.begin(name.c_str(),true);
  res = preferences.getString(key.c_str() ,"");
  preferences.end();
  return res;
}

void claer_ALL_EEprom(String name){
 //ลบ
  preferences.begin(name.c_str(), false);
  preferences.clear();
  preferences.end();
}

#endif