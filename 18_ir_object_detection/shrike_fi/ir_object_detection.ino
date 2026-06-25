/*
  Smart IR Object Detection System - Shrike Fi (ESP32-S3)

  A simple and efficient IR-based object detection system.
  The system detects the presence of an object using an IR sensor
  and triggers an LED and a buzzer alert in real-time.

  Wiring for Shrike Fi (ESP32-S3):
    IR Sensor OUT - ESP_IO1 (GPIO 1)
    LED           - ESP_IO2 (GPIO 2)
    Buzzer        - ESP_IO3 (GPIO 3)
*/

// pins
const int irSensorPin = 1;
const int ledPin      = 2;
const int buzzerPin   = 3;

void setup() {
  Serial.begin(115200);

  // init pins
  pinMode(irSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // turn off led and buzzer on boot
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  Serial.println("System Initialized...");
}

void loop() {
  // read ir sensor (low = object detected)
  int irState = digitalRead(irSensorPin);

  if (irState == LOW) {
    // object detected
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Object Detected!");
  } else {
    // no object
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    Serial.println("Monitoring...");
  }

  delay(200); // small delay to avoid spamming serial
}
