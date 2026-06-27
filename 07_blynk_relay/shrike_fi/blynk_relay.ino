/*
  Blynk Smart Thermostat Relay - Shrike Fi (ESP32-S3)

  Cloud-connected smart thermostat using Blynk IoT platform and a DHT22 sensor.
  It constantly monitors temperature (V0) and humidity (V1). When the temperature
  crosses a defined threshold, it automatically triggers an edge-triggered relay change.
  
  You can manually override the state using the physical button on the board, or 
  remotely using the V2 switch widget in the Blynk app. The edge-triggered logic
  ensures the thermostat won't fight your manual overrides until the temp crosses the threshold again!
  It also supports Active-Low relay modules.

  Wiring:
    Relay signal - ESP_IO3 (GPIO 3)
    Manual button - ESP_IO14 (GPIO 14, active low, internal pullup)
    Status LED   - ESP_IO21 (GPIO 21)
    DHT22 Data   - ESP_IO4 (GPIO 4)

  Needs: Blynk library, DHTesp library
*/

#define BLYNK_TEMPLATE_ID   "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME   "Shrike Relay"
#define BLYNK_AUTH_TOKEN     "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// pins
#define RELAY_PIN  3
#define BUTTON_PIN 14
#define LED_PIN    21
#define DHT_PIN    4

DHTesp dht;
BlynkTimer timer;

bool relayState       = false;
int  lastButtonState  = HIGH;
int  buttonState      = HIGH;
unsigned long lastDebounce = 0;
const unsigned long DEBOUNCE_MS = 50;

// Thermostat threshold
const float TEMP_THRESHOLD = 30.0; // °C

// Set to true if your relay module turns ON when the pin is LOW
#define ACTIVE_LOW_RELAY true

// called from blynk app when V2 changes
BLYNK_WRITE(V2) {
  int val = param.asInt();
  relayState = (val == 1);
  applyRelayState();
  Serial.print("blynk V2: relay ");
  Serial.println(relayState ? "on" : "off");
}

void applyRelayState() {
  bool pinState = relayState;
  if (ACTIVE_LOW_RELAY) {
    pinState = !pinState; // invert for active low relays
  }
  digitalWrite(RELAY_PIN, pinState ? HIGH : LOW);
  digitalWrite(LED_PIN,   relayState ? HIGH : LOW); // keep LED active-high
  Blynk.virtualWrite(V2, relayState ? 1 : 0);
}

void readSensor() {
  float h = dht.getHumidity();
  float t = dht.getTemperature();

  if (dht.getStatus() != DHTesp::ERROR_NONE) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send to Blynk
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print("C  Hum: ");
  Serial.print(h);
  Serial.println("%");

  // Thermostat logic (Edge triggered)
  static bool lastTempState = (t > TEMP_THRESHOLD); // Initialize on first run
  bool currentTempState = (t > TEMP_THRESHOLD);
  
  if (currentTempState != lastTempState) {
    relayState = currentTempState;
    applyRelayState();
    Serial.print("Thermostat triggered relay ");
    Serial.println(relayState ? "ON" : "OFF");
    lastTempState = currentTempState;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN,  OUTPUT);
  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  bool initialPinState = false;
  if (ACTIVE_LOW_RELAY) initialPinState = true;
  digitalWrite(RELAY_PIN, initialPinState ? HIGH : LOW); // Start OFF
  digitalWrite(LED_PIN,   LOW);

  dht.setup(DHT_PIN, DHTesp::DHT22);
  timer.setInterval(2000L, readSensor);

  Serial.println("connecting to blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("connected");
}

void loop() {
  Blynk.run();
  timer.run();
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

        Serial.print("button toggle: relay ");
        Serial.println(relayState ? "on" : "off");
      }
    }
  }
  lastButtonState = reading;
}
