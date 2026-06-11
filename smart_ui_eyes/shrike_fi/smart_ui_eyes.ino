/*
 * Smart UI Eyes - Shrike Fi (ESP32-S3)
 * ====================================
 * Animates programmatic "robot eyes" on an SSD1306 OLED, 
 * simulating blinking and looking around. Fetches weather data.
 * 
 * Dependencies:
 * - Adafruit SSD1306
 * - Adafruit GFX Library
 * - ArduinoJson
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configuration ---
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String apiKey = "YOUR_OPENWEATHERMAP_API_KEY";
String city = "New York";
String countryCode = "US";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// ── OLED SPI pins (Shrike Fi – ESP32-S3) ──
#define OLED_MOSI 35   // ESP_IO35
#define OLED_CLK  36   // ESP_IO36
#define OLED_DC   37   // ESP_IO37
#define OLED_RST  38   // ESP_IO38
#define OLED_CS   34   // ESP_IO34

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

String weatherStr = "-- C";
unsigned long lastWeatherFetch = 0;

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.println("Connecting...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void fetchWeather() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    StaticJsonDocument<1024> doc;
    if (!deserializeJson(doc, payload)) {
      float temp = doc["main"]["temp"];
      weatherStr = String(temp, 1) + " C";
    }
  }
  http.end();
}

void drawEyes(int xOffset = 0, int yOffset = 0, int blinkAmount = 0) {
  display.clearDisplay();
  
  // Left Eye (Base width: 30, Base height: 40)
  int lx = 20 + xOffset;
  int ly = 10 + yOffset + blinkAmount;
  int lh = max(2, 40 - (blinkAmount * 2));
  
  // Right Eye
  int rx = 78 + xOffset;
  int ry = 10 + yOffset + blinkAmount;
  int rh = max(2, 40 - (blinkAmount * 2));
  
  // Draw rounded rects for cute eyes
  display.fillRoundRect(lx, ly, 30, lh, 8, SSD1306_WHITE);
  display.fillRoundRect(rx, ry, 30, rh, 8, SSD1306_WHITE);
  
  // Display weather at the very bottom
  display.setTextSize(1);
  display.setCursor(40, 55);
  display.print(weatherStr);
  
  display.display();
}

void loop() {
  if (millis() - lastWeatherFetch > 600000 || lastWeatherFetch == 0) {
    fetchWeather();
    lastWeatherFetch = millis();
  }
  
  int action = random(0, 10);
  
  if (action < 2) {
    // Blink
    for (int b = 0; b <= 20; b += 5) {
      drawEyes(0, 0, b);
      delay(20);
    }
    for (int b = 20; b >= 0; b -= 5) {
      drawEyes(0, 0, b);
      delay(20);
    }
  } else if (action < 4) {
    // Look left
    drawEyes(-10, 0, 0);
    delay(1000);
    drawEyes(0, 0, 0);
  } else if (action < 6) {
    // Look right
    drawEyes(10, 0, 0);
    delay(1000);
    drawEyes(0, 0, 0);
  } else {
    // Idle
    drawEyes(0, 0, 0);
  }
  
  delay(500);
}
