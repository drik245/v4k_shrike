/*
  Smart UI Eyes - Shrike Fi (ESP32-S3)

  Interactive OLED UI with animated robot eyes, touch-based menu,
  NTP clock, and live weather data.

  States: WELCOME > EYES (idle) > MENU > TIME / WEATHER > back to EYES

  Touch controls:
    NEXT   (ESP_IO1, Touch IO 1) - cycle menu / manual NTP sync
    SELECT (ESP_IO2, Touch IO 2) - select option / go back

  OLED SPI wiring:
    MOSI-35, CLK-36, DC-37, RST-38, CS-34

  Needs: Adafruit SSD1306, Adafruit GFX, ArduinoJson
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String apiKey      = "YOUR_OPENWEATHERMAP_API_KEY";
String city        = "New York";
String countryCode = "US";

const char* ntpServer     = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   dstOffset_sec = 0;

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64

// oled spi pins
#define OLED_MOSI 35
#define OLED_CLK  36
#define OLED_DC   37
#define OLED_RST  38
#define OLED_CS   34

// touch pins
#define TOUCH_NEXT   1
#define TOUCH_SELECT 2
#define TOUCH_THRESHOLD 30000

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

// state machine
enum State { ST_WELCOME, ST_EYES, ST_MENU, ST_TIME, ST_WEATHER };
State currentState = ST_WELCOME;

// touch debounce
bool lastNext = false, lastSelect = false;
unsigned long lastNextDebounce = 0, lastSelectDebounce = 0;
const unsigned long TOUCH_DEBOUNCE = 200;

// eyes animation
unsigned long lastEyeAction = 0;
unsigned long eyeActionInterval = 2000;
int eyeXoff = 0, eyeYoff = 0, eyeBlink = 0;
bool eyesBlinking = false;
int blinkPhase = 0;
unsigned long lastBlinkStep = 0;

// menu
int menuIndex = 0;
const int MENU_ITEMS = 2;
const char* menuLabels[] = {"Clock", "Weather"};

// weather cache
String weatherTemp = "--";
String weatherHum  = "--";
String weatherDesc = "--";
unsigned long lastWeatherFetch = 0;

// welcome timer
unsigned long welcomeStart = 0;


// touch helpers
bool readNext() {
  return touchRead(TOUCH_NEXT) < TOUCH_THRESHOLD;
}
bool readSelect() {
  return touchRead(TOUCH_SELECT) < TOUCH_THRESHOLD;
}
bool nextPressed() {
  bool cur = readNext();
  if (cur && !lastNext && (millis() - lastNextDebounce > TOUCH_DEBOUNCE)) {
    lastNextDebounce = millis();
    lastNext = cur;
    return true;
  }
  lastNext = cur;
  return false;
}
bool selectPressed() {
  bool cur = readSelect();
  if (cur && !lastSelect && (millis() - lastSelectDebounce > TOUCH_DEBOUNCE)) {
    lastSelectDebounce = millis();
    lastSelect = cur;
    return true;
  }
  lastSelect = cur;
  return false;
}


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

  // eyes
  display.fillRoundRect(lx, ey, 30, eh, 10, SSD1306_WHITE);
  display.fillRoundRect(rx, ey, 30, eh, 10, SSD1306_WHITE);

  // pupils (only when not blinking)
  if (eh > 10) {
    int px = 8 + constrain(xOff, -5, 5);
    int py = (eh / 2) - 4 + constrain(yOff, -3, 3);
    display.fillCircle(lx + px + 7, ey + py + 4, 4, SSD1306_BLACK);
    display.fillCircle(rx + px + 7, ey + py + 4, 4, SSD1306_BLACK);
  }

  // bottom bar
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


// eyes animation logic
void updateEyes() {
  if (eyesBlinking) {
    if (millis() - lastBlinkStep > 25) {
      lastBlinkStep = millis();
      if (blinkPhase < 22) {
        blinkPhase += 5;
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
  Serial.println("wifi connected");

  configTime(gmtOffset_sec, dstOffset_sec, ntpServer);

  struct tm t;
  int retries = 0;
  while (!getLocalTime(&t) && retries < 10) { delay(500); retries++; }

  fetchWeather();

  delay(max(0L, 3000L - (long)(millis() - welcomeStart)));
  currentState = ST_EYES;
}

void loop() {
  switch (currentState) {

    case ST_EYES:
      updateEyes();
      if (nextPressed()) {
        eyesBlinking = true; blinkPhase = 0; lastBlinkStep = millis();
      }
      if (selectPressed()) {
        menuIndex = 0;
        currentState = ST_MENU;
        drawMenu();
      }
      break;

    case ST_MENU:
      if (nextPressed()) {
        menuIndex = (menuIndex + 1) % MENU_ITEMS;
        drawMenu();
      }
      if (selectPressed()) {
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

    case ST_TIME:
      drawTime();
      if (nextPressed()) {
        configTime(gmtOffset_sec, dstOffset_sec, ntpServer);
      }
      if (selectPressed()) {
        currentState = ST_EYES;
      }
      delay(1000);
      break;

    case ST_WEATHER:
      if (nextPressed()) {
        fetchWeather();
        drawWeather();
      }
      if (selectPressed()) {
        currentState = ST_EYES;
      }
      break;

    default:
      currentState = ST_EYES;
      break;
  }
}
