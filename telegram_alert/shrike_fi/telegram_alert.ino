/*
 * Telegram Alert – Shrike Fi (ESP32-S3)
 * =====================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Uses an IR sensor (active LOW) to detect motion and sends a
 * Telegram message via the Bot API. Includes a cooldown timer,
 * an LED flash on detection, and periodic incoming message checks
 * so you can send /status to query the device.
 *
 * Wiring (Shrike Fi header):
 *   IR sensor OUT → ESP_IO4  (GPIO 4, active LOW)
 *   Status LED    → ESP_IO21 (GPIO 21)
 *
 * Dependencies:
 *   UniversalTelegramBot, ArduinoJson
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// --- Configuration ---
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

#define BOTtoken  "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID   "YOUR_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// ── Hardware Pins (Shrike Fi) ──
#define IR_PIN   4   // ESP_IO4
#define LED_PIN  21  // ESP_IO21

volatile bool motionDetected = false;
unsigned long lastAlertTime = 0;
const unsigned long COOLDOWN_MS = 10000; // 10s between alerts

unsigned long lastBotCheck = 0;
const unsigned long BOT_CHECK_MS = 5000; // check incoming messages every 5s

unsigned long motionCount = 0; // total detections since boot

void IRAM_ATTR onMotion() {
  motionDetected = true;
}

void flashLED(int times, int ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(ms);
    digitalWrite(LED_PIN, LOW);
    delay(ms);
  }
}

void handleNewMessages(int numNew) {
  for (int i = 0; i < numNew; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text    = bot.messages[i].text;
    text.toLowerCase();

    if (text == "/status") {
      String msg = "🟢 Shrike Fi Online\n";
      msg += "Motion count: " + String(motionCount) + "\n";
      msg += "Uptime: " + String(millis() / 60000) + " min";
      bot.sendMessage(chat_id, msg, "");
    } else if (text == "/start") {
      bot.sendMessage(chat_id, "👋 Shrike Fi Motion Alert\nCommands:\n/status — device info", "");
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN,  INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(IR_PIN), onMotion, FALLING);

  Serial.print("[TG] Connecting to WiFi...");
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[TG] WiFi connected");

  flashLED(3, 100);
  bot.sendMessage(CHAT_ID, "✅ Shrike Fi: Motion Alert System Started!", "");
}

void loop() {
  // ── Motion detection ──
  if (motionDetected) {
    motionDetected = false;

    if (millis() - lastAlertTime > COOLDOWN_MS) {
      motionCount++;
      lastAlertTime = millis();

      Serial.println("[TG] Motion! Sending alert...");
      flashLED(2, 80);
      bot.sendMessage(CHAT_ID,
        "🚨 ALERT: Motion detected! (#" + String(motionCount) + ")", "");
    } else {
      Serial.println("[TG] Motion ignored (cooldown)");
    }
  }

  // ── Incoming message check ──
  if (millis() - lastBotCheck > BOT_CHECK_MS) {
    lastBotCheck = millis();
    int numNew = bot.getUpdates(bot.last_message_received + 1);
    while (numNew) {
      handleNewMessages(numNew);
      numNew = bot.getUpdates(bot.last_message_received + 1);
    }
  }
}
