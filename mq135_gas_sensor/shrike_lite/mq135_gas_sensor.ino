/*
 * MQ-135 Gas Sensor - Shrike Lite (RP2040)
 * ========================================
 * Reads the analog output from an MQ-135 Air Quality Sensor
 * and prints a basic quality status to the Serial Monitor.
 */

// RP2040 ADC pins: 26 (A0), 27 (A1), 28 (A2)
#define MQ135_PIN 26

void setup() {
  Serial.begin(115200);
  
  // RP2040 ADC resolution is 12-bit (0-4095) in Arduino core
  analogReadResolution(12);
  
  Serial.println("Starting MQ-135 Air Quality Sensor (Shrike Lite)...");
  delay(2000); // Allow sensor to warm up a bit
}

void loop() {
  int rawValue = analogRead(MQ135_PIN);
  
  Serial.print("Raw MQ-135 Value: ");
  Serial.println(rawValue);
  
  if (rawValue < 1000) {
    Serial.println("Status: Good Air Quality");
  } else if (rawValue < 2000) {
    Serial.println("Status: Poor Air Quality (Warning)");
  } else {
    Serial.println("Status: DANGER - High Gas Concentration");
  }
  
  Serial.println("------------------------------");
  delay(2000);
}
