/*
  NTP Digital Clock - Shrike Fi (ESP32-S3)

  Connects to WiFi, syncs with NTP, and shows the time on an
  SSD1306 OLED (SPI) with date, day of week, large time display,
  and a seconds progress bar.

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

  // border
  display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);

  // day of week
  display.setTextSize(1);
  int dayLen = strlen(DAYS[ti.tm_wday]) * 6;
  display.setCursor((128 - dayLen) / 2, 4);
  display.print(DAYS[ti.tm_wday]);

  // date
  display.setCursor(34, 16);
  display.print(dateBuf);

  // time (large)
  display.setTextSize(2);
  display.setCursor(8, 28);
  display.print(timeBuf);

  // seconds progress bar
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
  Serial.println("\nwifi connected");

  display.clearDisplay();
  display.setCursor(10, 28);
  display.println("Syncing NTP...");
  display.display();

  configTime(gmtOffset_sec, daylightOffset_s, ntpServer);

  // wait for first sync
  struct tm t;
  int retries = 0;
  while (!getLocalTime(&t) && retries < 10) {
    delay(500);
    retries++;
  }
  Serial.println("time synced");
}

void loop() {
  showTime();
  delay(1000);
}
