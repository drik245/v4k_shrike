/*
 * MQ-135 Gas Sensor - Shrike Fi (ESP32-S3)
 * ========================================
 * Reads the analog output from an MQ-135 Air Quality Sensor
 * and prints a basic quality status to the Serial Monitor.
 */

#define MQ135_PIN 1 // ESP32-S3 ADC pin

void setup() {
  Serial.begin(115200);
  
  // Set ADC attenuation for 3.3V range
  analogSetPinAttenuation(MQ135_PIN, ADC_11db);
  
  Serial.println("Starting MQ-135 Air Quality Sensor...");
  delay(2000); // Allow sensor to warm up a bit
}

void loop() {
  // ESP32-S3 default ADC resolution is 12-bit (0-4095)
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
