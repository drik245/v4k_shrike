#include <WiFi.h>
#include <PubSubClient.h> // Install "PubSubClient" by Nick O'Leary
#include <DHT.h>          // Install "DHT sensor library" by Adafruit

#define DHTPIN 14 // ESP_IO14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "192.168.1.100"; // Your MQTT Broker IP

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    if (client.connect("ShrikeClient")) {
      Serial.println("MQTT Connected");
    } else {
      delay(5000);
      return;
    }
  }
  client.loop();

  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    
    if(!isnan(t)) {
      client.publish("shrike/temperature", String(t).c_str());
      client.publish("shrike/humidity", String(h).c_str());
      Serial.println("Published Temp & Humidity");
    }
  }
}
