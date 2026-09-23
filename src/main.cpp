#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "time.h"
#include <HTTPClient.h>
#include<PubSubClient.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <data_include.h>

#include <./Network_config/WebServerSetup.h>
#include <./Network_config/config_network.h>
#include <./Network_config/esp32_Ap.h>

#include <./Mqtt_service/setup_mqtt.h>
#include <./Mqtt_service/pub_Active.h>

#define rfid_RxPin 15
#define rfid_TxPin 16

String setDeviceId() {
    String mac = WiFi.macAddress();

    mac.replace(":", "");
    Serial.println("Mac Address: " + mac);
    wifiInfo.mac = mac;
    return  mac;
}

bool checkMemory_SSID_Pass(){
    preferences.begin("wifi-creds", true);
    wifiInfo.EEPROM_SSID = preferences.getString("ssid", "");
    wifiInfo.EEPROM_Pass = preferences.getString("pass", "");
    preferences.end();
    //ดู EEprom 
    if (wifiInfo.EEPROM_SSID.length() > 0) {
        Serial.println("Find SSID in EEPROM: " + wifiInfo.EEPROM_SSID);
        if(wifiInfo.EEPROM_Pass.length() > 0){
            Serial.println("Find Password in EEPROM: " + wifiInfo.EEPROM_Pass);
        } else {
            Serial.println("No Password to connect WiFi");
        }
        return true;
    }
    return false;
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, rfid_RxPin, rfid_TxPin);
    delay(3000);
    while (!Serial) { 
        delay(10);
    }

    if (!LittleFS.begin(true)) {
        Serial.println("An Error has occurred while mounting LittleFS");
    }
    WiFi.mode(WIFI_MODE_STA);

    wifiInfo.deviceId = setDeviceId();
    setAP(&wifiInfo.deviceId);
    server.begin();
    setupWebServer();

    //เช็ค SSID / password in EEprom
    if(!checkMemory_SSID_Pass()) {
        Serial.println("No SSID in EEPROM, start WebServer to setup WiFi");
    }

    Serial.println("Connecting to WiFi: " + wifiInfo.EEPROM_SSID);
    connectWiFi(&wifiInfo.EEPROM_SSID, &wifiInfo.EEPROM_Pass);
    Serial.print("connect internet...");
    while (WiFi.status() != WL_CONNECTED) {
        loopWebServer();
        delay(500);
    }
    
    configTime(7*3600, 0, "pool.ntp.org");
    client.setServer(mqtt_server, mqtt_port); //อยู่ใน inculde
}

void loop() {
    loopWebServer();
    if(WiFi.status() != WL_CONNECTED){
        ESP.restart();
    }
    if (!client.connected()) {
      testMQTT_connect();   
    }
    client.loop();

    static std::vector<uint8_t> frameBuffer;

    while (Serial2.available() > 0) {
        uint8_t value = Serial2.read();
        
        if (frameBuffer.empty() && value != 0x52) continue; 
        frameBuffer.push_back(value);

        if (frameBuffer.size() == 28) {
            
            Serial.print("Data incoming: ");
            for (uint8_t b : frameBuffer) {
                if (b < 0x10) Serial.print("0");
                Serial.print(b, HEX);
                Serial.print(" ");
            }
            Serial.println();

            // 🎯 [แก้ไขจุดนี้] เช็กตำแหน่งที่ 2 และ 5 ให้ตรงตามข้อมูลจริง
            if (frameBuffer[2] == 0x02 && frameBuffer[5] == 0x80) {
                
                // แกะรหัส EPC 12 Bytes (24 ตัวอักษร) เริ่มจากตำแหน่ง byte ที่ 12
                String currentEpc = "";
                for (int i = 12; i < 24; i++) {
                    if (frameBuffer[i] < 0x10) currentEpc += "0";
                    currentEpc += String(frameBuffer[i], HEX);
                }
                currentEpc.toUpperCase();

                if (currentEpc == "E28069950000401636C74999") {
                    Serial.println("➔ [พบสินค้า A] : สั่งเปิดไฟสีเขียว 🟢");
                } 
                else if (currentEpc == "E28069950000400C84B2D120") {
                    Serial.println("➔ [พบสินค้า B] : สั่งเปิดไฟสีแดง 🔴");
                } 
                else if (currentEpc == "E28069950000401636C76D99") { 
                    Serial.println("➔ [พบสินค้า C] : สั่งเปิดไฟสีเหลือง 🟡");
                }
                else {
                    Serial.printf("➔ พบแท็กใบอื่น ๆ รหัสคือ: %s\n", currentEpc.c_str());
                }
                testPub();
                sendDataToAPI(currentEpc);
            }
            
            frameBuffer.clear(); 
        }
    }

    static uint32_t lastByteTime = 0;
    if (Serial2.available() > 0) lastByteTime = millis();
    if (!frameBuffer.empty() && (millis() - lastByteTime > 50)) {
        frameBuffer.clear();
    }

    delay(1); 
}