/*
 * DHT11 Web Server - Shrike Fi (ESP32-S3)
 * ========================================
 * Serves a webpage displaying temperature and humidity from a DHT11 sensor.
 * 
 * Dependencies:
 * - DHT sensor library by Adafruit
 * - Adafruit Unified Sensor
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// --- Configuration ---
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

#define DHTPIN 4      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11
#define LED_PIN 21    // Onboard LED

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

void handleRoot() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    server.send(500, "text/plain", "Failed to read from DHT sensor!");
    return;
  }

  String html = "<!DOCTYPE html><html><head><title>Shrike Weather</title>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<meta http-equiv=\"refresh\" content=\"3\">"; // Auto-refresh every 3s
  html += "<style>body{font-family: Arial, sans-serif; text-align: center; background-color: #121212; color: white; padding-top: 50px;}";
  html += ".card{background-color: #1e1e1e; border-radius: 10px; padding: 30px; display: inline-block; box-shadow: 0 4px 8px rgba(0,0,0,0.5);}";
  html += "h1{color: #00bcd4;} .value{font-size: 2.5em; font-weight: bold; margin: 10px 0;}";
  html += ".temp{color: #ff5722;} .hum{color: #2196f3;}</style></head><body>";
  html += "<div class=\"card\"><h1>Shrike Weather Station</h1>";
  html += "<p>Temperature: <span class=\"value temp\">" + String(t, 1) + "&deg;C</span></p>";
  html += "<p>Humidity: <span class=\"value hum\">" + String(h, 1) + "%</span></p>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  dht.begin();

  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  digitalWrite(LED_PIN, HIGH);
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
