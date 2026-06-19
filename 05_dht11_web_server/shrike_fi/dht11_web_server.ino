/*
 * DHT11 Web Server – Shrike Fi (ESP32-S3)
 * ========================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Serves a styled webpage with real-time temperature, humidity,
 * and heat index from a DHT11 sensor. Also exposes a JSON API
 * endpoint at /api for programmatic access.
 *
 * Wiring (Shrike Fi header):
 *   DHT11 data → ESP_IO4  (GPIO 4)
 *   Onboard LED → ESP_IO21 (GPIO 21)
 *
 * Dependencies:
 *   DHT sensor library (Adafruit), Adafruit Unified Sensor
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// --- Configuration ---
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ── Hardware Pins (Shrike Fi) ──
#define DHTPIN   4      // ESP_IO4
#define DHTTYPE  DHT11
#define LED_PIN  21     // ESP_IO21 (Onboard LED)

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// ── Cached readings ──
float lastTemp = NAN;
float lastHum  = NAN;
float lastHI   = NAN;
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 2000; // DHT11 min sample period

void readSensor() {
  if (millis() - lastReadTime < READ_INTERVAL) return;
  lastReadTime = millis();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    lastHum  = h;
    lastTemp = t;
    lastHI   = dht.computeHeatIndex(t, h, false); // Celsius
  }
}

// ── JSON API ──
void handleAPI() {
  readSensor();
  if (isnan(lastTemp)) {
    server.send(503, "application/json", "{\"error\":\"sensor not ready\"}");
    return;
  }
  String json = "{\"temperature_c\":" + String(lastTemp, 1)
              + ",\"humidity\":" + String(lastHum, 1)
              + ",\"heat_index_c\":" + String(lastHI, 1) + "}";
  server.send(200, "application/json", json);
}

// ── Web UI ──
void handleRoot() {
  readSensor();

  String t = isnan(lastTemp) ? "--" : String(lastTemp, 1);
  String h = isnan(lastHum)  ? "--" : String(lastHum, 1);
  String hi = isnan(lastHI)  ? "--" : String(lastHI, 1);

  String html = R"rawliteral(
<!DOCTYPE html><html><head>
<title>Shrike Weather Station</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv="refresh" content="3">
<style>
  *{margin:0;padding:0;box-sizing:border-box}
  body{font-family:'Segoe UI',sans-serif;background:#0f0f0f;color:#eee;
       display:flex;justify-content:center;align-items:center;min-height:100vh}
  .card{background:#1a1a2e;border-radius:16px;padding:40px 50px;
        box-shadow:0 8px 32px rgba(0,0,0,.6);text-align:center;min-width:300px}
  h1{font-size:1.4em;color:#00bcd4;margin-bottom:24px;letter-spacing:1px}
  .row{margin:16px 0}
  .label{font-size:.85em;color:#888;text-transform:uppercase;letter-spacing:2px}
  .value{font-size:2.8em;font-weight:700;margin:4px 0}
  .temp{color:#ff5722} .hum{color:#2196f3} .hi{color:#ff9800}
  .unit{font-size:.5em;color:#666}
  hr{border:none;border-top:1px solid #2a2a3a;margin:12px 0}
</style></head><body>
<div class="card">
  <h1>🌡️ Shrike Weather Station</h1>
  <div class="row"><div class="label">Temperature</div>
    <div class="value temp">)rawliteral" + t + R"rawliteral(<span class="unit">°C</span></div></div>
  <hr>
  <div class="row"><div class="label">Humidity</div>
    <div class="value hum">)rawliteral" + h + R"rawliteral(<span class="unit">%</span></div></div>
  <hr>
  <div class="row"><div class="label">Heat Index</div>
    <div class="value hi">)rawliteral" + hi + R"rawliteral(<span class="unit">°C</span></div></div>
</div></body></html>)rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();

  Serial.println("\n[DHT11] Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_PIN, HIGH);
  Serial.println("\n[DHT11] WiFi connected!");
  Serial.print("[DHT11] IP: ");
  Serial.println(WiFi.localIP());

  server.on("/",    handleRoot);
  server.on("/api", handleAPI);
  server.begin();
  Serial.println("[DHT11] HTTP server started — open http://" + WiFi.localIP().toString());
}

void loop() {
  server.handleClient();
}
