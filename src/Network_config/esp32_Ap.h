#include <Arduino.h>
#include <WiFi.h>

inline void setAP(String *deviceId) {
    WiFi.setHostname(("iiot-mini-" + *deviceId).c_str());

    WiFi.mode(WIFI_MODE_AP);
    String apSsid_ = "TTX-RFID-" + deviceId->substring(deviceId->length() - 6);
    String apPassword_ = "SPU-" + deviceId->substring(deviceId->length() - 6);

    if(!WiFi.softAP(apSsid_.c_str(), apPassword_.c_str())){
        Serial.println("AP mode failed to start");
        return;
    }
    Serial.println("AP mode started");
    Serial.println("SSID: " + apSsid_ + " Password: " + apPassword_);
    Serial.print("AP IP address: " + WiFi.softAPIP().toString());
    Serial.println("\tHostname: " + String(WiFi.getHostname())); 
}