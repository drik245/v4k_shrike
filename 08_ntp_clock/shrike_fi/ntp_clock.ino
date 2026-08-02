/*
  NTP Digital Clock - Shrike Fi (ESP32-S3)

  Connects to WiFi, syncs with NTP, and shows the time on an
  SSD1306 OLED (SPI) with date, day of week, large time display,
  and a seconds progress bar. Includes an internal oscillator fallback 
  if WiFi/NTP fails, and resyncs when WiFi returns.

  OLED SPI wiring:
    MOSI - ESP_IO35, CLK - ESP_IO36, DC - ESP_IO37,
    RST  - ESP_IO38, CS  - ESP_IO34

  Needs: Adafruit SSD1306, Adafruit GFX
*/

#include <WiFi.h>
#include "time.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* ntpServer        = "pool.ntp.org";
const long  gmtOffset_sec    = 19800;  // IST +5:30
const int   daylightOffset_s = 0;

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64

// oled spi pins
#define OLED_MOSI 35
#define OLED_CLK  36
#define OLED_DC   37
#define OLED_RST  38
#define OLED_CS   34

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

const char* DAYS[] = {"Sunday","Monday","Tuesday","Wednesday",
                      "Thursday","Friday","Saturday"};

// Internal fallback time
int hours = 12;
int minutes = 0;
int seconds = 0;
int currentDay = 0; // 0 = Sunday
unsigned long lastTick = 0;
unsigned long lastWiFiCheck = 0;
bool wifiWasConnected = false;

void tickInternalClock() {
  if (millis() - lastTick >= 1000) {
    lastTick += 1000;
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
          currentDay = (currentDay + 1) % 7;
        }
      }
    }
  }
}

void showTime() {
  struct tm ti;
  bool isNtpSynced = false;
  
  if (getLocalTime(&ti, 0) && ti.tm_year > 100) { // Year > 2000 implies NTP sync
    isNtpSynced = true;
    // Sync internal clock to NTP so they stay matched
    hours = ti.tm_hour;
    minutes = ti.tm_min;
    seconds = ti.tm_sec;
    currentDay = ti.tm_wday;
  }
  
  char timeBuf[9];
  sprintf(timeBuf, "%02d:%02d:%02d", hours, minutes, seconds);

  char dateBuf[15];
  if (isNtpSynced) {
    strftime(dateBuf, sizeof(dateBuf), "%d-%m-%Y", &ti);
  } else {
    strcpy(dateBuf, "Offline Mode");
  }

  display.clearDisplay();

  // border
  display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);

  // day of week
  display.setTextSize(1);
  int dayLen = strlen(DAYS[currentDay]) * 6;
  display.setCursor((128 - dayLen) / 2, 4);
  display.print(DAYS[currentDay]);

  // date / offline status
  if (isNtpSynced) {
    display.setCursor(34, 16);
  } else {
    display.setCursor(28, 16);
  }
  display.print(dateBuf);

  // time (large)
  display.setTextSize(2);
  display.setCursor(8, 28);
  display.print(timeBuf);

  // seconds progress bar
  int barW = map(seconds, 0, 59, 0, 118);
  display.fillRoundRect(5, 52, barW, 6, 2, SSD1306_WHITE);
  display.drawRoundRect(5, 52, 118, 6, 2, SSD1306_WHITE);

  display.display();
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 28);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Try connecting for 5 seconds before falling back
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    configTime(gmtOffset_sec, daylightOffset_s, ntpServer);
    wifiWasConnected = true;
  } else {
    Serial.println("\nWiFi failed. Starting offline fallback mode.");
  }

  lastTick = millis();
}

void loop() {
  // Update internal clock fallback
  tickInternalClock();
  
  // Periodically check and reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    wifiWasConnected = false;
    if (millis() - lastWiFiCheck > 10000) {
      lastWiFiCheck = millis();
      Serial.println("Attempting to reconnect WiFi...");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
    }
  } else {
    if (!wifiWasConnected) {
      Serial.println("WiFi reconnected! Starting NTP sync.");
      configTime(gmtOffset_sec, daylightOffset_s, ntpServer);
      wifiWasConnected = true;
    }
  }

  showTime();
  delay(100);
}
