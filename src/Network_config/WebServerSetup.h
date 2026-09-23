WebServer server(80);

 // ยิง Request ไปขอ status
void handleStatus() {
    preferences.begin("wifi-creds", true); // โหมดอ่าน
    String savedSSID = preferences.getString("ssid", "");
    preferences.end();

    bool hasSaved = (savedSSID.length() > 0);
    String json = "{\"connected\": false, \"hasSavedCredentials\": " + 
                  String(hasSaved ? "true" : "false") + 
                  ", \"ssid\": \"" + savedSSID + "\"}";
    
    server.send(200, "application/json", json);
}
  // Request ไปขอ scan WiFi
void handleScan() {
    WiFi.scanDelete();
    int n = WiFi.scanNetworks();
    String json = "[";
    for (int i = 0; i < n; ++i) {
        if (i > 0) json += ",";
        json += "\"" + WiFi.SSID(i) + "\"";
    }
    json += "]";
    
    server.send(200, "application/json", json);
}

// รับค่า SSID/Password จากหน้าเว็บเพื่อบันทึก
void handleConnect() {
    String ssid = server.arg("ssid");
    String pass = server.arg("password");

    if (ssid.length() > 0) {
        preferences.begin("wifi-creds", false); // โหมดเขียน
        preferences.putString("ssid", ssid);
        preferences.putString("pass", pass);
        preferences.end();

        server.send(200, "text/plain", "Saved successfully");
        
        delay(1000);
        ESP.restart(); // restart เพื่อจะได้เิอาค่าจาก EEprom 
    } else {
        server.send(400, "text/plain", "SSID cannot be empty");
    }
}

void handleLogout(){
    preferences.begin("wifi-creds", false);
    preferences.clear();
    preferences.end();

    server.send(200, "text/plain", "Cleared");
    
    delay(1000);
    ESP.restart(); // restart เพื่อเริ่มใหม่ จะได้เรียก AP
}

void setupWebServer() {
    Serial.println("Starting WebServer for WiFi setup...");
    if (!LittleFS.begin(true)) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    // Server ส่งไฟล์ HTML/CSS/JS อัตโนมัติเมื่อมี request
    server.serveStatic("/", LittleFS, "/web_connet/index.html");
    server.serveStatic("/style.css", LittleFS, "/web_connet/style.css");
    server.serveStatic("/script.js", LittleFS, "/web_connet/script.js");

    server.on("/status", HTTP_GET, handleStatus);
    server.on("/scan", HTTP_GET, handleScan);
    server.on("/connect", HTTP_POST, handleConnect);
    server.on("/logout", HTTP_POST, handleLogout);

    Serial.println("HTTP server started");
}

void sendDataToAPI(String rfid_tag) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        String serverPath = "http://172.20.10.3:3000/api/db/equipment"; 
        http.begin(serverPath);
        
        // 3. ตั้งค่า Header ว่าเราจะส่งข้อมูลแบบ JSON
        http.addHeader("Content-Type", "application/json");
        
        JsonDocument doc;
        doc["deviceId"] = wifiInfo.deviceId; 
        doc["IP"] = wifiInfo.IP;
        doc["epc"] = rfid_tag;       
        doc["timestamp"] = gettime();
        
        String jsonPayload;
        serializeJson(doc, jsonPayload);
        
        Serial.println("Sending API Payload: " + jsonPayload);

        int httpResponseCode = http.POST(jsonPayload);
        
        // 6. เช็คผลลัพธ์การตอบกลับจาก Server
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            Serial.println("Response from server: " + response);
        } else {
            Serial.print("Error code on sending POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi Disconnected, cannot send API");
    }
}



void loopWebServer() {
    server.handleClient(); // คอยฟังสัญญาณ HTTP จาก Client
}