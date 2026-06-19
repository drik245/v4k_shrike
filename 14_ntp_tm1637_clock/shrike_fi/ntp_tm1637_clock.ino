/*
 * NTP TM1637 Clock – Shrike Fi (ESP32-S3)
 * ========================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Internet-synced digital clock displayed on a TM1637 4-digit
 * seven-segment display. Time is fetched via NTP and shown in
 * HH:MM format with a blinking colon every second.
 *
 * Wiring (Shrike Fi header):
 *   TM1637 CLK → ESP_IO1 (GPIO 1)
 *   TM1637 DIO → ESP_IO2 (GPIO 2)
 *   TM1637 VCC → 3.3V
 *   TM1637 GND → GND
 *
 * Dependencies:
 *   TM1637Display library (by Avishay Orpaz)
 */

#include <WiFi.h>
#include "time.h"
#include <TM1637Display.h>

const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* ntpServer     = "pool.ntp.org";
const long  gmtOffset_sec = 19800;  // IST +5:30
const int   dstOffset_sec = 0;

#define TM_CLK  1   // ESP_IO1
#define TM_DIO  2  // ESP_IO2

TM1637Display tm(TM_CLK, TM_DIO);

bool colonOn = true;
unsigned long lastToggle = 0;

void setup() {
  Serial.begin(115200);

  tm.setBrightness(5); // 0-7
  
  // Show "conn" while connecting
  uint8_t conn[] = {
    SEG_A | SEG_D | SEG_E | SEG_F,           // C
    SEG_C | SEG_D | SEG_E | SEG_G,           // o
    SEG_C | SEG_E | SEG_G,                    // n
    SEG_C | SEG_E | SEG_G                     // n
  };
  tm.setSegments(conn);

  Serial.print("[NTP-TM] Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[NTP-TM] WiFi connected");

  // Show "Sync" while waiting for NTP
  uint8_t sync[] = {
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,   // S
    SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,   // y
    SEG_C | SEG_E | SEG_G,                    // n
    SEG_A | SEG_D | SEG_E | SEG_F             // C
  };
  tm.setSegments(sync);

  configTime(gmtOffset_sec, dstOffset_sec, ntpServer);

  // Wait for first NTP sync
  struct tm t;
  int retries = 0;
  while (!getLocalTime(&t) && retries < 20) {
    delay(500);
    retries++;
  }
  Serial.println("[NTP-TM] Time synced!");
}

void loop() {
  struct tm ti;
  if (!getLocalTime(&ti)) {
    // Show dashes if time not available
    uint8_t dash[] = { SEG_G, SEG_G, SEG_G, SEG_G };
    tm.setSegments(dash);
    delay(1000);
    return;
  }

  // Build HH:MM number for showNumberDecEx
  int displayVal = ti.tm_hour * 100 + ti.tm_min;

  // Toggle colon every 500ms
  if (millis() - lastToggle >= 500) {
    lastToggle = millis();
    colonOn = !colonOn;
  }

  // showNumberDecEx(num, dots, leading_zeros, length, pos)
  // dots: 0b01000000 = colon between digit 2 and 3
  uint8_t dots = colonOn ? 0b01000000 : 0x00;
  tm.showNumberDecEx(displayVal, dots, true);

  // Also log to serial every second
  static int lastSec = -1;
  if (ti.tm_sec != lastSec) {
    lastSec = ti.tm_sec;
    char buf[9];
    strftime(buf, sizeof(buf), "%H:%M:%S", &ti);
    Serial.print("[NTP-TM] ");
    Serial.println(buf);
  }

  delay(50); // fast loop for smooth colon blink
}
