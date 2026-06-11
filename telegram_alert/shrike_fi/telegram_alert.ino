/*
 * Telegram Alert - Shrike Fi (ESP32-S3)
 * =====================================
 * Uses an IR Sensor to detect motion and sends a message to a Telegram Bot.
 * 
 * Dependencies:
 * - UniversalTelegramBot library
 * - ArduinoJson library
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// --- Configuration ---
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Initialize Telegram BOT
#define BOTtoken "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// ── IR Sensor Pin (Shrike Fi) ──
#define IR_PIN 4 // ESP_IO4
bool motionDetected = false;

void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT_PULLUP);
  // Attach interrupt to IR PIN (falling edge means object detected)
  attachInterrupt(digitalPinToInterrupt(IR_PIN), detectsMovement, FALLING);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  bot.sendMessage(CHAT_ID, "Shrike Fi: Motion Alert System Started!", "");
}

void loop() {
  if (motionDetected) {
    Serial.println("Motion Detected! Sending Telegram Alert...");
    bot.sendMessage(CHAT_ID, "ALERT: Motion detected by Shrike Fi IR sensor!", "");
    
    motionDetected = false;
    
    // Wait before sending another alert
    delay(5000);
  }
}
