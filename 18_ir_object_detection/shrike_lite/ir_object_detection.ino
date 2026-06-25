/*
  Smart IR Object Detection System - Shrike Lite (RP2040)

  A simple and efficient IR-based object detection system.
  The system detects the presence of an object using an IR sensor
  and triggers an LED and a buzzer alert in real-time.

  Wiring for Shrike Lite (RP2040):
    IR Sensor OUT - RP_IO26 (GPIO 26)
    LED           - RP_IO27 (GPIO 27)
    Buzzer        - RP_IO28 (GPIO 28)
*/

// pins
const int irSensorPin = 26;
const int ledPin      = 27;
const int buzzerPin   = 28;

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
