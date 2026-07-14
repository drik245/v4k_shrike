#include <Arduino.h>

#define SENSOR_PIN 14 // RP_IO14 (Digital Sound Sensor Output)
#define RELAY_PIN  15 // RP_IO15 (To Relay Module IN pin)

bool relayState = false;
unsigned long lastClapTime = 0;
const unsigned long debounceDelay = 500; // Ignore echos/bounces for 500ms after a clap

void setup() {
  Serial.begin(115200);
  
  pinMode(SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Start with relay off
  
  Serial.println("Clap Switch Ready! (Shrike Lite)");
  Serial.println("Waiting for a loud sound...");
}

void loop() {
  int soundDetected = digitalRead(SENSOR_PIN);
  
  if (soundDetected == HIGH) {
    if (millis() - lastClapTime > debounceDelay) {
      relayState = !relayState;
      digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
      
      Serial.print("Clap Detected! Relay is now: ");
      Serial.println(relayState ? "ON" : "OFF");
      
      lastClapTime = millis();
    }
  }
}
