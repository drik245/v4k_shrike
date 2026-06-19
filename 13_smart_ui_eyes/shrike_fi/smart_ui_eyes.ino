/*
 * Smart UI Eyes – Shrike Fi (ESP32-S3)
 * ====================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Interactive OLED UI with animated robot eyes, touch-based menu
 * navigation, NTP clock, and live weather data.
 *
 * States:
 *   WELCOME → EYES (idle) → MENU → TIME / WEATHER → back to EYES
 *
 * Touch controls:
 *   NEXT   (ESP_IO1, Touch IO 1) — cycle menu / manual NTP sync
 *   SELECT (ESP_IO2, Touch IO 2) — select option / go back
 *
 * OLED SPI wiring (Shrike Fi header):
 *   MOSI → ESP_IO35, CLK → ESP_IO36, DC → ESP_IO37,
 *   RST  → ESP_IO38, CS  → ESP_IO34
 *
 * Dependencies:
 *   Adafruit SSD1306, Adafruit GFX, ArduinoJson
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//   Configuration  
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String apiKey      = "YOUR_OPENWEATHERMAP_API_KEY";
String city        = "New York";
String countryCode = "US";

const char* ntpServer     = "pool.ntp.org";
const long  gmtOffset_sec = 19800;  // IST +5:30
const int   dstOffset_sec = 0;

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64

#define OLED_MOSI 35
#define OLED_CLK  36
#define OLED_DC   37
#define OLED_RST  38
#define OLED_CS   34

//   Touch pins  
#define TOUCH_PIN   1   // ESP_IO1
#define TOUCH_THRESHOLD 30000
#define DEBOUNCE 50
#define DOUBLE_TAP 300

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

//   State machine  
enum State { ST_WELCOME, ST_EYES, ST_MENU, ST_TIME, ST_WEATHER };
State currentState = ST_WELCOME;

//   Touch  
bool lastTouch = false;
bool waitingSecond = false;
unsigned long lastTapTime = 0, lastReleaseTime = 0;

//   Eyes animation  
unsigned long lastEyeAction = 0;
unsigned long eyeActionInterval = 2000;
int eyeXoff = 0, eyeYoff = 0, eyeBlink = 0;
bool eyesBlinking = false;
int blinkPhase = 0;
unsigned long lastBlinkStep = 0;

//   Menu  
int menuIndex = 0;
const int MENU_ITEMS = 2;
const char* menuLabels[] = {"Clock", "Weather"};

//   Weather cache  
String weatherTemp = "--";
String weatherHum  = "--";
String weatherDesc = "--";
unsigned long lastWeatherFetch = 0;

//   Welcome timer  
unsigned long welcomeStart = 0;

//  Touch helpers  
int readTap() {
  static unsigned long lastReadTime = 0;
  unsigned long now = millis();

  if (now - lastReadTime < 20) return 0;
  lastReadTime = now;

  bool cur = touchRead(TOUCH_PIN) < TOUCH_THRESHOLD;
  int result = 0;

  if (cur && !lastTouch && (now - lastReleaseTime > DEBOUNCE)) {
    if (waitingSecond && (now - lastTapTime < DOUBLE_TAP)) {
      result = 2; waitingSecond = false;
    } else {
      lastTapTime = now; waitingSecond = true;
    }
  }
  if (waitingSecond && !cur && (now - lastTapTime > DOUBLE_TAP)) {
    result = 1; waitingSecond = false;
  }
  if (!cur && lastTouch) lastReleaseTime = now;
  lastTouch = cur;
  return result;
}
               
//  Network 
void fetchWeather() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q="
             + city + "," + countryCode
             + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  int code = http.GET();
  if (code > 0) {
    String payload = http.getString();
    StaticJsonDocument<1024> doc;
    if (!deserializeJson(doc, payload)) {
      float t = doc["main"]["temp"];
      int   h = doc["main"]["humidity"];
      weatherTemp = String(t, 1) + " C";
      weatherHum  = String(h) + "%";
      weatherDesc = doc["weather"][0]["description"].as<String>();
    }
  }
  http.end();
  lastWeatherFetch = millis();
}

// Drawing functions 
void drawWelcome() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(16, 8);
  display.print("Shrike");
  display.setTextSize(1);
  display.setCursor(32, 30);
  display.print("Smart UI Eyes");
  display.setCursor(20, 48);
  display.print("Connecting...");
  display.display();
}

void drawEyes(int xOff, int yOff, int blink) {
  display.clearDisplay();

  int lx = 20 + xOff;
  int rx = 78 + xOff;
  int ey = 8 + yOff + blink;
  int eh = max(2, 44 - blink * 2);

  // Eyes with rounded corners
  display.fillRoundRect(lx, ey, 30, eh, 10, SSD1306_WHITE);
  display.fillRoundRect(rx, ey, 30, eh, 10, SSD1306_WHITE);

  // Pupils (only when not blinking)
  if (eh > 10) {
    int px = 8 + constrain(xOff, -5, 5);
    int py = (eh / 2) - 4 + constrain(yOff, -3, 3);
    display.fillCircle(lx + px + 7, ey + py + 4, 4, SSD1306_BLACK);
    display.fillCircle(rx + px + 7, ey + py + 4, 4, SSD1306_BLACK);
  }

  // Bottom status bar
  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print("[NEXT]");
  display.setCursor(86, 56);
  display.print("[MENU]");

  display.display();
}

void drawMenu() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(38, 2);
  display.print("-- MENU --");
  display.drawLine(0, 12, 128, 12, SSD1306_WHITE);

  for (int i = 0; i < MENU_ITEMS; i++) {
    int y = 20 + i * 18;
    if (i == menuIndex) {
      display.fillRoundRect(4, y - 2, 120, 14, 3, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    display.setCursor(12, y);
    display.print(menuLabels[i]);
  }
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 56);
  display.print("[NEXT]");
  display.setCursor(78, 56);
  display.print("[SELECT]");

  display.display();
}

void drawTime() {
  struct tm ti;
  if (!getLocalTime(&ti)) {
    display.clearDisplay();
    display.setCursor(10, 28);
    display.print("No NTP sync");
    display.display();
    return;
  }

  char timeBuf[9], dateBuf[11];
  strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &ti);
  strftime(dateBuf, sizeof(dateBuf), "%d-%m-%Y", &ti);

  display.clearDisplay();
  display.drawRoundRect(0, 0, 128, 52, 4, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(34, 6);
  display.print(dateBuf);

  display.setTextSize(2);
  display.setCursor(8, 22);
  display.print(timeBuf);

  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print("[SYNC]");
  display.setCursor(86, 56);
  display.print("[BACK]");

  display.display();
}

void drawWeather() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(city);

  display.setTextSize(2);
  display.setCursor(0, 14);
  display.print(weatherTemp);

  display.setTextSize(1);
  display.setCursor(0, 36);
  display.print("Hum: ");
  display.print(weatherHum);

  display.setCursor(0, 48);
  String d = weatherDesc;
  if (d.length() > 21) d = d.substring(0, 21);
  display.print(d);

  display.setCursor(86, 56);
  display.print("[BACK]");

  display.display();
}

//  Eyes animation 
void updateEyes() {
  // Blink animation 
  if (eyesBlinking) {
    if (millis() - lastBlinkStep > 25) {
      lastBlinkStep = millis();
      if (blinkPhase < 22) {
        blinkPhase = min(22, blinkPhase + 5);  // ← cap at 22
      } else {
        blinkPhase -= 5;
        if (blinkPhase <= 0) {
          blinkPhase = 0;
          eyesBlinking = false;
  }
}
      eyeBlink = blinkPhase;
    }
    drawEyes(eyeXoff, eyeYoff, eyeBlink);
    return;
  }

  if (millis() - lastEyeAction > eyeActionInterval) {
    lastEyeAction = millis();
    eyeActionInterval = random(1500, 4000);

    int action = random(0, 10);
    if (action < 3) {
      // Blink
      eyesBlinking = true;
      blinkPhase = 0;
      lastBlinkStep = millis();
    } else if (action < 5) {
      eyeXoff = -8;
      eyeYoff = 0;
    } else if (action < 7) {
      eyeXoff = 8;
      eyeYoff = 0;
    } else if (action < 8) {
      eyeXoff = 0;
      eyeYoff = -4;
    } else {
      eyeXoff = 0;
      eyeYoff = 0;
    }
  }

  drawEyes(eyeXoff, eyeYoff, 0);
}

//  Setup & Loop 
void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.setTextColor(SSD1306_WHITE);

  currentState = ST_WELCOME;
  welcomeStart = millis();
  drawWelcome();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("[EYES] WiFi connected");

  configTime(gmtOffset_sec, dstOffset_sec, ntpServer);

  // Wait for NTP
  struct tm t;
  int retries = 0;
  while (!getLocalTime(&t) && retries < 10) { delay(500); retries++; }

  fetchWeather();

  delay(max(0L, 3000L - (long)(millis() - welcomeStart)));
  currentState = ST_EYES;
}

void loop() {
  int tap = readTap();
  switch (currentState) {

    case ST_EYES:
      updateEyes();
      if (tap == 2) {
        // doesn't do anything special, just triggers a blink
        eyesBlinking = true; blinkPhase = 0; lastBlinkStep = millis();
      }
      if (tap == 1) {
        menuIndex = 0;
        currentState = ST_MENU;
        drawMenu();
      }
      break;

    case ST_MENU:
      if (tap == 2) {
        menuIndex = (menuIndex + 1) % MENU_ITEMS;
        drawMenu();
      }
      if (tap == 1) {
        if (menuIndex == 0) {
          currentState = ST_TIME;
          drawTime();
        } else {
          currentState = ST_WEATHER;
          if (millis() - lastWeatherFetch > 60000) fetchWeather();
          drawWeather();
        }
      }
      break;

    case ST_TIME: {
      static unsigned long lastTimeDraw = 0;
      if (millis() - lastTimeDraw > 1000) {
        lastTimeDraw = millis();
        drawTime();
      }
      if (tap == 2) configTime(gmtOffset_sec, dstOffset_sec, ntpServer);
      if (tap == 1) currentState = ST_EYES;
      break;
    }

    case ST_WEATHER:
      if (tap == 2) {
        fetchWeather();
        drawWeather();
      }
      if (tap == 1) {
        currentState = ST_EYES;
      }
      break;

    default:
      currentState = ST_EYES;
      break;
  }
}
