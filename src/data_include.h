#ifndef DATA_INCLUDE_H
#define DATA_INCLUDE_H

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

#endif