#include "../data_include.h"

void connectWiFi(String *ssid = nullptr, String *password = nullptr) {
    if (ssid != nullptr && *ssid != "") {
        if (password != nullptr) {
            WiFi.begin(ssid->c_str(), password->c_str());
        }else {
            WiFi.begin(ssid->c_str());
        }
        //time out 30 sec เอง
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            delay(500);
            Serial.print(".");
            Serial.print("5555");
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✅ Connected to AP Success");
        } else {
            Serial.println("\n❌ Failed to connect to WiFi, restarting...");

            preferences.begin("wifi-creds", false);
            preferences.clear();
            preferences.end();
            ESP.restart();
        }

        wifiInfo.subnet = WiFi.subnetMask().toString();
        wifiInfo.gateway = WiFi.gatewayIP().toString();
        wifiInfo.IP = WiFi.localIP().toString();
        wifiInfo.StartTime = gettime();
        Serial.print("\nConnected! IP: " + WiFi.localIP().toString());
        Serial.println("\tSubnet: " + wifiInfo.subnet);
        Serial.print("Gateway: " + wifiInfo.gateway);
        Serial.println("\tStart Time: " + wifiInfo.StartTime);
    } else {
        Serial.println("not find to SSID");
    }
}


// void connectWiFi(String *ssid = nullptr, String *password = nullptr) {
//     if (ssid != nullptr && *ssid != "") {
//         if (password != nullptr) {
//             WiFi.begin(ssid->c_str(), password->c_str());
//         } else {
//             WiFi.begin(ssid->c_str());
//         }
//         while (WiFi.status() != WL_CONNECTED) {
//             delay(500);
//             Serial.print(".");
//         }
//         Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
//     } else {
//         Serial.println("Invalid SSID");
//     }
// }