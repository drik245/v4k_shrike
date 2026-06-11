/*
 * Blynk Relay Control - Shrike Fi (ESP32-S3)
 * ==========================================
 * Connects to the Blynk Cloud to toggle a digital relay pin.
 * 
 * Dependencies:
 * - Blynk library by Volodymyr Shymanskyy
 */

#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME "Shrike Relay"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// ── Relay Pin (Shrike Fi) ──
#define RELAY_PIN 4 // ESP_IO4

// This function is called every time the Virtual Pin 0 state changes in the Blynk app
BLYNK_WRITE(V0) {
  int pinValue = param.asInt(); // assigning incoming value from pin V0 to a variable
  
  if (pinValue == 1) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Relay ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Relay OFF");
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
}
