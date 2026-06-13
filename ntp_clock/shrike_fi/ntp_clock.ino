/*
 * NTP Digital Clock – Shrike Fi (ESP32-S3)
 * ========================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Connects to WiFi, syncs with NTP, and displays the time on an
 * SSD1306 OLED (SPI) with a date row, large time, day-of-week,
 * and a seconds progress bar.
 *
 * OLED SPI wiring (Shrike Fi header):
 *   MOSI → ESP_IO35  (GPIO 35)
 *   CLK  → ESP_IO36  (GPIO 36)
 *   DC   → ESP_IO37  (GPIO 37)
 *   RST  → ESP_IO38  (GPIO 38)
 *   CS   → ESP_IO34  (GPIO 34)
 *
 * Dependencies:
 *   Adafruit SSD1306, Adafruit GFX
 */

#include <WiFi.h>
#include "time.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configuration ---
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* ntpServer        = "pool.ntp.org";
const long  gmtOffset_sec    = 19800;  // IST +5:30
const int   daylightOffset_s = 0;

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

const char* DAYS[] = {"Sunday","Monday","Tuesday","Wednesday",
                      "Thursday","Friday","Saturday"};

void showTime() {
  struct tm ti;
  if (!getLocalTime(&ti)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 28);
    display.println("Waiting for NTP...");
    display.display();
    return;
  }

  char timeBuf[9];
  strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &ti);

  char dateBuf[11];
  strftime(dateBuf, sizeof(dateBuf), "%d-%m-%Y", &ti);

  display.clearDisplay();

  // ── Border ──
  display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);

  // ── Day of week ──
  display.setTextSize(1);
  int dayLen = strlen(DAYS[ti.tm_wday]) * 6;
  display.setCursor((128 - dayLen) / 2, 4);
  display.print(DAYS[ti.tm_wday]);

  // ── Date ──
  display.setCursor(34, 16);
  display.print(dateBuf);

  // ── Time (large) ──
  display.setTextSize(2);
  display.setCursor(8, 28);
  display.print(timeBuf);

  // ── Seconds progress bar ──
  int barW = map(ti.tm_sec, 0, 59, 0, 118);
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

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[NTP] WiFi connected");

  display.clearDisplay();
  display.setCursor(10, 28);
  display.println("Syncing NTP...");
  display.display();

  configTime(gmtOffset_sec, daylightOffset_s, ntpServer);

  // Wait for first sync
  struct tm t;
  int retries = 0;
  while (!getLocalTime(&t) && retries < 10) {
    delay(500);
    retries++;
  }
  Serial.println("[NTP] Time synced");
}

void loop() {
  showTime();
  delay(1000);
}
