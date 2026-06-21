/*
  NTP TM1637 Clock - Shrike Fi (ESP32-S3)

  Internet-synced digital clock on a TM1637 4-digit display.
  Shows HH:MM with a blinking colon. Time fetched via NTP.

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

const char* ntpServer     = "pool.ntp.org";
const long  gmtOffset_sec = 19800;  // IST +5:30
const int   dstOffset_sec = 0;

#define TM_CLK  1
#define TM_DIO  2

TM1637Display tm(TM_CLK, TM_DIO);

bool colonOn = true;
unsigned long lastToggle = 0;

void setup() {
  Serial.begin(115200);

  tm.setBrightness(5);

  // show "conn" while connecting
  uint8_t conn[] = {
    SEG_A | SEG_D | SEG_E | SEG_F,
    SEG_C | SEG_D | SEG_E | SEG_G,
    SEG_C | SEG_E | SEG_G,
    SEG_C | SEG_E | SEG_G
  };
  tm.setSegments(conn);

  Serial.print("connecting to wifi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nwifi connected");

  // show "Sync" while waiting for NTP
  uint8_t sync[] = {
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
    SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,
    SEG_C | SEG_E | SEG_G,
    SEG_A | SEG_D | SEG_E | SEG_F
  };
  tm.setSegments(sync);

  configTime(gmtOffset_sec, dstOffset_sec, ntpServer);

  struct tm t;
  int retries = 0;
  while (!getLocalTime(&t) && retries < 20) {
    delay(500);
    retries++;
  }
  Serial.println("time synced");
}

void loop() {
  struct tm ti;
  if (!getLocalTime(&ti)) {
    uint8_t dash[] = { SEG_G, SEG_G, SEG_G, SEG_G };
    tm.setSegments(dash);
    delay(1000);
    return;
  }

  int displayVal = ti.tm_hour * 100 + ti.tm_min;

  // toggle colon every 500ms
  if (millis() - lastToggle >= 500) {
    lastToggle = millis();
    colonOn = !colonOn;
  }

  uint8_t dots = colonOn ? 0b01000000 : 0x00;
  tm.showNumberDecEx(displayVal, dots, true);

  // log to serial every second
  static int lastSec = -1;
  if (ti.tm_sec != lastSec) {
    lastSec = ti.tm_sec;
    char buf[9];
    strftime(buf, sizeof(buf), "%H:%M:%S", &ti);
    Serial.println(buf);
  }

  delay(50);
}
