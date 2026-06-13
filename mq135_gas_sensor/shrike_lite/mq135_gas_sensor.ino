/*
 * MQ-135 Gas Sensor – Shrike Lite (RP2040)
 * ========================================
 * Board target : Raspberry Pi Pico (Generic RP2040)
 * Core         : arduino-pico (Earle Philhower)
 *
 * Reads the analog output from an MQ-135 Air Quality Sensor with
 * multi-sample averaging for stable readings. Uses onboard LED
 * (RP_IO4) as a warning indicator.
 *
 * Wiring (Shrike Lite header):
 *   MQ-135 AO   → RP_IO29  (GPIO 29, ADC3)
 *   Warning LED  → RP_IO4   (GPIO 4, Onboard LED)
 *
 * No external libraries required.
 */

// ── Hardware Pins (Shrike Lite) ──
#define MQ135_PIN  29  // RP_IO29 – ADC3
#define LED_PIN     4  // RP_IO4  – onboard LED

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

  analogReadResolution(12); // 0-4095

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("[MQ135] Starting Air Quality Sensor (Shrike Lite)...");
  Serial.println("[MQ135] Warming up (20s recommended for accuracy)...");
  delay(2000);
}

void loop() {
  int raw = readAveraged();

  AirQuality quality;
  const char* statusStr;

  if (raw < GOOD_LIMIT) {
    quality   = AIR_GOOD;
    statusStr = "GOOD";
    digitalWrite(LED_PIN, LOW);
  } else if (raw < WARN_LIMIT) {
    quality   = AIR_POOR;
    statusStr = "POOR (Warning)";
    digitalWrite(LED_PIN, HIGH);
  } else {
    quality   = AIR_DANGER;
    statusStr = "DANGER!";
    digitalWrite(LED_PIN, HIGH);
  }

  Serial.print("[MQ135] Avg: ");
  Serial.print(raw);
  Serial.print("  Status: ");
  Serial.println(statusStr);

  delay(2000);
}
