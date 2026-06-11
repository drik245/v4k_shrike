/*
 * NTP Digital Clock - Shrike Fi (ESP32-S3)
 * ========================================
 * Connects to WiFi, syncs with an NTP server, and displays the time on an OLED.
 * 
 * Dependencies:
 * - Adafruit SSD1306
 * - Adafruit GFX Library
 */

#include <WiFi.h>
#include "time.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configuration ---
const char* ssid       = "YOUR_WIFI_NAME";
const char* password   = "YOUR_WIFI_PASSWORD";

const char* ntpServer  = "pool.ntp.org";
const long  gmtOffset_sec = 19800; // India Standard Time (+5:30) = 5.5 * 3600
const int   daylightOffset_sec = 0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Shrike Fi Default I2C
#define I2C_SDA 14
#define I2C_SCL 15

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    display.clearDisplay();
    display.setCursor(0, 30);
    display.println("Failed to obtain time");
    display.display();
    return;
  }

  char timeStr[9];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  
  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);

  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, WHITE);
  
  display.setTextSize(1);
  display.setCursor(32, 15);
  display.println(dateStr);
  
  display.setTextSize(2);
  display.setCursor(16, 35);
  display.println(timeStr);
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  
  Wire.begin(I2C_SDA, I2C_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("Connecting to WiFi...");
  display.display();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  display.clearDisplay();
  display.setCursor(0, 30);
  display.println("Syncing NTP Time...");
  display.display();
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  printLocalTime();
  delay(1000);
}
