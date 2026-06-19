/*
 * Blynk Relay Control – Shrike Fi (ESP32-S3)
 * ==========================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Cloud-controlled relay using the Blynk IoT platform.
 * Features:
 *   - Virtual Pin V0 from Blynk app toggles the relay
 *   - Physical button on ESP_IO3 for manual override
 *   - LED indicator on ESP_IO21 mirrors relay state
 *   - Syncs physical toggle back to the Blynk app widget
 *
 * Wiring (Shrike Fi header):
 *   Relay signal → ESP_IO4  (GPIO 4)
 *   Manual button → ESP_IO3  (GPIO 3, active LOW with pull-up)
 *   Status LED   → ESP_IO21 (GPIO 21)
 *
 * Dependencies:
 *   Blynk library by Volodymyr Shymanskyy
 */

#define BLYNK_TEMPLATE_ID   "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME   "Shrike Relay"
#define BLYNK_AUTH_TOKEN     "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// ── Hardware Pins (Shrike Fi) ──
#define RELAY_PIN  4   // ESP_IO4
#define BUTTON_PIN 3   // ESP_IO3  (manual override)
#define LED_PIN    21  // ESP_IO21 (status indicator)

bool relayState       = false;
int  lastButtonState  = HIGH;
int  buttonState      = HIGH;
unsigned long lastDebounce = 0;
const unsigned long DEBOUNCE_MS = 50;

// Called from Blynk app when V0 changes
BLYNK_WRITE(V0) {
  int val = param.asInt();
  relayState = (val == 1);
  applyRelayState();
  Serial.print("[BLYNK] V0 → Relay ");
  Serial.println(relayState ? "ON" : "OFF");
}

void applyRelayState() {
  digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
  digitalWrite(LED_PIN,   relayState ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN,  OUTPUT);
  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN,   LOW);

  Serial.println("[RELAY] Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("[RELAY] Connected!");
}

void loop() {
  Blynk.run();
  handleButton();
}

void handleButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounce = millis();
  }

  if ((millis() - lastDebounce) > DEBOUNCE_MS) {
    if (reading != buttonState) {
      buttonState = reading;

      // Falling edge → button pressed (active LOW)
      if (buttonState == LOW) {
        relayState = !relayState;
        applyRelayState();

        // Push state back to Blynk widget
        Blynk.virtualWrite(V0, relayState ? 1 : 0);
        Serial.print("[BTN] Manual toggle → Relay ");
        Serial.println(relayState ? "ON" : "OFF");
      }
    }
  }
  lastButtonState = reading;
}
