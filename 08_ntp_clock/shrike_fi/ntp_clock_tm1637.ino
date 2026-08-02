/*
  NTP Digital Clock (TM1637 Version) - Shrike Fi (ESP32-S3)

  Connects to WiFi, syncs with NTP, and shows the time on a
  TM1637 4-digit seven-segment display. Includes an internal 
  oscillator fallback if WiFi/NTP fails, and resyncs when WiFi returns.

  Wiring:
    TM1637 CLK - ESP_IO1 (GPIO 1)
    TM1637 DIO - ESP_IO2 (GPIO 2)

  Needs: TM1637Display library (by Avishay Orpaz)
*/

#include <WiFi.h>
#include "time.h"
#include <TM1637Display.h>

const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* ntpServer        = "pool.ntp.org";
const long  gmtOffset_sec    = 19800;  // IST +5:30
const int   daylightOffset_s = 0;

// pins
#define TM_CLK 1
#define TM_DIO 2

TM1637Display tm(TM_CLK, TM_DIO);

// Internal fallback time
int hours = 12;
int minutes = 0;
int seconds = 0;
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
        hours = (hours + 1) % 24;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  tm.setBrightness(5);
  
  // Show "----" while connecting to WiFi
  uint8_t dash[] = { SEG_G, SEG_G, SEG_G, SEG_G };
  tm.setSegments(dash);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  
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
    Serial.println("\nWiFi connected.");
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

  // Check if NTP has synced
  struct tm ti;
  if (getLocalTime(&ti, 0) && ti.tm_year > 100) { 
    // Update internal clock to match NTP
    hours = ti.tm_hour;
    minutes = ti.tm_min;
    seconds = ti.tm_sec;
  }
  
  // Display time
  int timeVal = hours * 100 + minutes;
  
  // Blink colon every second
  bool showColon = (millis() / 500) % 2 == 0;
  uint8_t dots = showColon ? 0b01000000 : 0x00;
  
  tm.showNumberDecEx(timeVal, dots, true);
  
  delay(100);
}
