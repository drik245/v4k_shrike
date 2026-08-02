/*
  NTP Digital Clock (TM1637 Version) - Shrike Fi (ESP32-S3)

  Connects to WiFi, syncs with NTP, and shows the time on a
  TM1637 4-digit seven-segment display.

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

void setup() {
  Serial.begin(115200);

  tm.setBrightness(5);
  
  // Show "----" while connecting to WiFi
  uint8_t dash[] = { SEG_G, SEG_G, SEG_G, SEG_G };
  tm.setSegments(dash);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  configTime(gmtOffset_sec, daylightOffset_s, ntpServer);

  // Wait for time to sync
  struct tm ti;
  while (!getLocalTime(&ti)) {
    Serial.println("Waiting for NTP sync...");
    delay(1000);
  }
  Serial.println("Time synced.");
}

void loop() {
  struct tm ti;
  if (getLocalTime(&ti)) {
    int timeVal = ti.tm_hour * 100 + ti.tm_min;
    
    // Blink colon every second
    bool showColon = (millis() / 500) % 2 == 0;
    uint8_t dots = showColon ? 0b01000000 : 0x00;
    
    tm.showNumberDecEx(timeVal, dots, true);
  }
  delay(100);
}
