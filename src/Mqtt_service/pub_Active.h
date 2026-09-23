

void testMQTT_connect(){
    bool connected = false;
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    client.setServer(mqtt_server, mqtt_port);

    connected = client.connect(clientId.c_str(), mqtt_user, mqtt_pass);
    if (connected) {
      Serial.println("connected MQTT");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
}

void testPub(){
    if (client.connected()) {
        client.publish(topic_pub, "1");
        Serial.println("Publish status succeed");
    }else{
        Serial.println("❌ Publish status failed, MQTT not connected");
        return;
  }    
}