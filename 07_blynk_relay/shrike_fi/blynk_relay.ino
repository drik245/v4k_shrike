/*
  Blynk Relay Control - Shrike Fi (ESP32-S3)

  Cloud-controlled relay using Blynk IoT platform.
  V0 widget in the app toggles the relay. Physical button
  on the board overrides and syncs state back to Blynk.
  LED mirrors relay state.

  Wiring:
    Relay signal - ESP_IO4 (GPIO 4)
    Manual button - ESP_IO3 (GPIO 3, active low, internal pullup)
    Status LED   - ESP_IO21 (GPIO 21)

  Needs: Blynk library (by Volodymyr Shymanskyy)
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

// pins
#define RELAY_PIN  4
#define BUTTON_PIN 3
#define LED_PIN    21

bool relayState       = false;
int  lastButtonState  = HIGH;
int  buttonState      = HIGH;
unsigned long lastDebounce = 0;
const unsigned long DEBOUNCE_MS = 50;

// called from blynk app when V0 changes
BLYNK_WRITE(V0) {
  int val = param.asInt();
  relayState = (val == 1);
  applyRelayState();
  Serial.print("blynk V0: relay ");
  Serial.println(relayState ? "on" : "off");
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

  Serial.println("connecting to blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("connected");
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

      if (buttonState == LOW) {
        relayState = !relayState;
        applyRelayState();

        // push state back to blynk widget
        Blynk.virtualWrite(V0, relayState ? 1 : 0);
        Serial.print("button toggle: relay ");
        Serial.println(relayState ? "on" : "off");
      }
    }
  }
  lastButtonState = reading;
}
