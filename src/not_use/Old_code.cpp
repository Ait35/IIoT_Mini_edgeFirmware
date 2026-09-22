// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <Arduino.h>
// #include <ArduinoJson.h>
// #include "time.h"

// const char* ssid = "Wokwi-GUEST";
// const char* password = "";

// //Mqtt config
// const char* mqtt_server = "broker.emqx.io";
// const int mqtt_port = 1883;
// const char* mqtt_user = "";    
// const char* mqtt_pass = "";     

// const char* topic_pub_status = "room999/cam1/status"; //topic Status
// const char* topic_sub = "test01";
// const char* topic_pub = "test001"; 

// #define RX_PIN 15  // ต่อกับขา TX ของ RFID
// #define TX_PIN 16  // ต่อกับขา RX ของ RFID

// WiFiClient espClient;
// PubSubClient client(espClient);

// struct tm timeinfo;

// struct device_info {
//   String device_name;
//   String device_mac;
//   String device_IP;
//   String device_topic_sub;
//   String device_status;
//   String device_start_time;
//   int device_couter;
// };

// String gettime() {
//   if (getLocalTime(&timeinfo)) {
//     char timeString[50];
//     strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);

//     Serial.println("Synced time: " + String(timeString));
//     return String(timeString);
//   } else {
//     Serial.println("Failed to obtain time");
//     return "";
//   }
// }

// unsigned long lastMsg = 0;
// int counter = 0;

// // ฟังก์ชันรับข้อความและแสดงผลข้อความอย่างเดียว
// void callback(char* topic, byte* payload, unsigned int length) {
//   String messageTemp = "";
//   for (int i = 0; i < length; i++) {
//     messageTemp += (char)payload[i];
//   }
//   Serial.print("Received on [");
//   Serial.print(topic);
//   Serial.print("]: ");
//   Serial.println(messageTemp);
// }

// // ฟังก์ชันเชื่อมต่อ MQTT และ Reconnect อัตโนมัติเมื่อหลุด
// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("(reconnection) Attempting MQTT connection...");
//     String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    
//     bool connected = false;
//     if (strlen(mqtt_user) > 0) {
//       connected = client.connect(clientId.c_str(), mqtt_user, mqtt_pass);
//     } else {
//       connected = client.connect(clientId.c_str());
//     }

//     if (connected) {
//       Serial.println("connected");
//       client.subscribe(topic_sub);
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       delay(5000);
//     }
//   }
// }

// //Publish ข้อมูล status อุปกรณ์
// void publisStatus() {
//   JsonDocument doc;
//   doc["device_name"] = "CAM1";
//   doc["device_mac"] = WiFi.macAddress();
//   doc["device_IP"] = WiFi.localIP().toString();
//   doc["device_topic_sub"] = topic_sub;
//   doc["device_status"] = "connected";
//   doc["device_start_time"] = gettime();
//   //doc["device_couter"] = counter;
  
//   String payload ;
//   serializeJson(doc, payload);

//   if (client.connected()) {
//     client.publish(topic_pub_status, payload.c_str());
//   }else{
//     Serial.println("❌ Publish status failed, MQTT not connected");
//     reconnect();
//   }
// }

// void setup() {
//   // Serial สำหรับแสดงผลบนคอมพิวเตอร์
//   Serial.begin(115200);

//   // --- เริ่มต้น Serial2 สำหรับอ่านค่า RFID ---
//   Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); 

//   Serial.println();
//   Serial.print("Connecting to Wi-Fi: ");
//   Serial.println(ssid);
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   configTime(7*3600, 0, "pool.ntp.org");

//   device_info device;
//   device.device_IP = WiFi.localIP().toString();
//   device.device_mac = WiFi.macAddress();
//   device.device_name = "CAM1";
//   device.device_topic_sub = topic_sub;
//   device.device_status = "connected";
//   // device.device_start_time = gettime();
//   device.device_couter = 0;

//   Serial.println(device.device_mac);
//   Serial.println("\nWiFi connected, IP: " + device.device_IP);

//   client.setServer(mqtt_server, mqtt_port);
//   client.setCallback(callback);

//   device.device_start_time = gettime();
// }


// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();
//   if (Serial2.available() > 0) {
//     // อ่านข้อมูลจาก RFID จนกว่าจะขึ้นบรรทัดใหม่
//     String rfidData = Serial2.readStringUntil('\n'); 
//     rfidData.trim(); // ตัดเว้นวรรคหรือสัญลักษณ์ซ่อนรูปด้านหน้าและหลังทิ้ง

//     // ถ้ามีข้อมูลแท็ก ให้ Publish ส่งเข้า MQTT ทันที
//     if (rfidData.length() > 0) {
//       Serial.println("RFID Tag Scanned: " + rfidData);
//       client.publish(topic_pub, "1");
//       publisStatus();
//     }
//   }
//   // ส่งข้อมูลนับเวลา (Publish) ทุกๆ 1 นาที (คงไว้ตามโค้ดเดิม)
//   unsigned long now = millis();
//   if (now - lastMsg > 60000) {
//     lastMsg = now;
//     counter++;
//     String payload = "System Uptime (min): " + String(counter);
//     Serial.println("Publishing: " + payload);
//   }

// }