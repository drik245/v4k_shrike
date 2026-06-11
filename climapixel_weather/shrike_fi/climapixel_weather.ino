/*
 * ClimaPixel Weather Display - Shrike Fi (ESP32-S3)
 * =================================================
 * Fetches current weather from OpenWeatherMap API and displays it on an SSD1306 OLED.
 * 
 * Dependencies:
 * - ArduinoJson
 * - Adafruit SSD1306
 * - Adafruit GFX Library
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

unsigned long lastTime = 0;
unsigned long timerDelay = 600000; // 10 minutes

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("WiFi Connected!");
  display.display();
  delay(1000);
  
  fetchWeather();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      fetchWeather();
    }
    lastTime = millis();
  }
}

void fetchWeather() {
  display.clearDisplay();
  display.setCursor(0, 30);
  display.println("Fetching Data...");
  display.display();

  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    
    float temp = doc["main"]["temp"];
    int humidity = doc["main"]["humidity"];
    String description = doc["weather"][0]["description"];
    
    display.clearDisplay();
    display.setTextSize(1);
    
    display.setCursor(0, 0);
    display.print("City: ");
    display.println(city);
    
    display.setCursor(0, 20);
    display.print("Temp: ");
    display.print(temp);
    display.println(" C");
    
    display.setCursor(0, 35);
    display.print("Hum:  ");
    display.print(humidity);
    display.println("%");
    
    display.setCursor(0, 50);
    display.println(description.substring(0, min(20, (int)description.length())));
    
    display.display();
  } else {
    Serial.println("Error on HTTP request");
    display.clearDisplay();
    display.setCursor(0, 30);
    display.println("Fetch Failed");
    display.display();
  }
  http.end();
}
