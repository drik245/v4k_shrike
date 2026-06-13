/*
 * ClimaPixel Weather Display – Shrike Fi (ESP32-S3)
 * =================================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Fetches current weather from OpenWeatherMap API and displays
 * temperature, humidity, description, and a graphical thermometer
 * bar on an SSD1306 OLED (SPI).
 *
 * OLED SPI wiring (Shrike Fi header):
 *   MOSI → ESP_IO35  (GPIO 35)
 *   CLK  → ESP_IO36  (GPIO 36)
 *   DC   → ESP_IO37  (GPIO 37)
 *   RST  → ESP_IO38  (GPIO 38)
 *   CS   → ESP_IO34  (GPIO 34)
 *
 * Dependencies:
 *   ArduinoJson, Adafruit SSD1306, Adafruit GFX
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configuration ---
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String apiKey      = "YOUR_OPENWEATHERMAP_API_KEY";
String city        = "New York";
String countryCode = "US";

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64

// ── OLED SPI pins (Shrike Fi – ESP32-S3) ──
#define OLED_MOSI 35   // ESP_IO35
#define OLED_CLK  36   // ESP_IO36
#define OLED_DC   37   // ESP_IO37
#define OLED_RST  38   // ESP_IO38
#define OLED_CS   34   // ESP_IO34

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

unsigned long lastFetch = 0;
const unsigned long FETCH_INTERVAL = 600000; // 10 minutes
bool firstFetchDone = false;

float  cachedTemp     = 0;
int    cachedHumidity = 0;
String cachedDesc     = "";
bool   dataValid      = false;

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 28);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[CLIMA] WiFi connected");

  fetchWeather();
  firstFetchDone = true;
  lastFetch = millis();
}

void loop() {
  if (millis() - lastFetch > FETCH_INTERVAL) {
    if (WiFi.status() == WL_CONNECTED) {
      fetchWeather();
    }
    lastFetch = millis();
  }
}

void fetchWeather() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 28);
  display.println("Fetching data...");
  display.display();

  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q="
             + city + "," + countryCode
             + "&appid=" + apiKey + "&units=metric";
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();

    StaticJsonDocument<1024> doc;
    DeserializationError err = deserializeJson(doc, payload);

    if (!err) {
      cachedTemp     = doc["main"]["temp"];
      cachedHumidity = doc["main"]["humidity"];
      cachedDesc     = doc["weather"][0]["description"].as<String>();
      dataValid      = true;
      showWeather();
    } else {
      Serial.print("[CLIMA] JSON error: ");
      Serial.println(err.c_str());
    }
  } else {
    Serial.print("[CLIMA] HTTP error: ");
    Serial.println(httpCode);
    display.clearDisplay();
    display.setCursor(20, 28);
    display.println("Fetch failed");
    display.display();
  }
  http.end();
}

void showWeather() {
  display.clearDisplay();

  // ── City name ──
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(city);

  // ── Temperature (large) ──
  display.setTextSize(2);
  display.setCursor(0, 14);
  display.print(cachedTemp, 1);
  display.setTextSize(1);
  display.print(" C");

  // ── Humidity ──
  display.setTextSize(1);
  display.setCursor(0, 36);
  display.print("Hum: ");
  display.print(cachedHumidity);
  display.print("%");

  // ── Description ──
  display.setCursor(0, 48);
  String desc = cachedDesc;
  if (desc.length() > 21) desc = desc.substring(0, 21);
  display.print(desc);

  // ── Thermometer bar (right side) ──
  int barX = 110;
  int barTop = 2;
  int barH = 56;
  display.drawRect(barX, barTop, 14, barH, SSD1306_WHITE);
  // Map temp range -10..50 → bar fill
  int fillH = constrain(map((int)(cachedTemp * 10), -100, 500, 0, barH - 4), 0, barH - 4);
  display.fillRect(barX + 2, barTop + (barH - 4) - fillH + 2, 10, fillH, SSD1306_WHITE);

  display.display();
}
