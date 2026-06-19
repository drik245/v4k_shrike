/*
 * MQ-135 Gas Sensor – Shrike Fi (ESP32-S3)
 * ========================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Reads the analog output from an MQ-135 Air Quality Sensor with
 * multi-sample averaging for stable readings. Includes an onboard
 * LED warning indicator and buzzer-ready output pin.
 *
 * Wiring (Shrike Fi header):
 *   MQ-135 AO  → ESP_IO1  (GPIO 1, ADC1_CH0)
 *   Warning LED → ESP_IO21 (GPIO 21, Onboard LED)
 *   Buzzer (opt) → ESP_IO3  (GPIO 3)
 *
 * No external libraries required.
 */

// ── Hardware Pins (Shrike Fi) ──
#define MQ135_PIN   1   // ESP_IO1 – ADC1_CH0
#define LED_PIN     21  // ESP_IO21 – onboard LED
#define BUZZER_PIN  3   // ESP_IO3 – optional buzzer

// ── ADC Averaging ──
#define NUM_SAMPLES   16
#define SAMPLE_DELAY  10  // ms between each sample

// ── Thresholds (tune after calibration in clean air) ──
#define GOOD_LIMIT    1000
#define WARN_LIMIT    2000

enum AirQuality { AIR_GOOD, AIR_POOR, AIR_DANGER };

int readAveraged() {
  long total = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    total += analogRead(MQ135_PIN);
    delay(SAMPLE_DELAY);
  }
  return (int)(total / NUM_SAMPLES);
}

void setup() {
  Serial.begin(115200);

  analogSetPinAttenuation(MQ135_PIN, ADC_11db); // 0–3.3V range

  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN,    LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("[MQ135] Starting Air Quality Sensor...");
  Serial.println("[MQ135] Warming up (20s recommended for accuracy)...");
  delay(2000); // short startup delay — real warm-up is ~20 min
}

void loop() {
  int raw = readAveraged();

  AirQuality quality;
  const char* statusStr;

  if (raw < GOOD_LIMIT) {
    quality   = AIR_GOOD;
    statusStr = "GOOD";
    digitalWrite(LED_PIN,    LOW);
    digitalWrite(BUZZER_PIN, LOW);
  } else if (raw < WARN_LIMIT) {
    quality   = AIR_POOR;
    statusStr = "POOR (Warning)";
    digitalWrite(LED_PIN, HIGH); // LED on as warning
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    quality   = AIR_DANGER;
    statusStr = "DANGER!";
    digitalWrite(LED_PIN,    HIGH);
    digitalWrite(BUZZER_PIN, HIGH); // optional buzzer on
  }

  Serial.print("[MQ135] Avg: ");
  Serial.print(raw);
  Serial.print("  Status: ");
  Serial.println(statusStr);

  delay(2000);
}
