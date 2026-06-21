/*
  MQ-135 Gas Sensor - Shrike Lite (RP2040)

  Reads analog output from MQ-135 with 16-sample averaging
  for stable readings. Onboard LED warns on poor/danger air.

  Wiring:
    MQ-135 AO   - RP_IO29 (GPIO 29, ADC3)
    Warning LED - RP_IO4 (GPIO 4, onboard LED)

  No external libraries needed.
*/

// pins
#define MQ135_PIN  29
#define LED_PIN     4

// averaging
#define NUM_SAMPLES   16
#define SAMPLE_DELAY  10

// thresholds (tune after calibration in clean air)
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

  analogReadResolution(12);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("starting MQ-135 sensor (shrike lite)...");
  Serial.println("warming up (20s recommended for accuracy)...");
  delay(2000);
}

void loop() {
  int raw = readAveraged();

  AirQuality quality;
  const char* statusStr;

  if (raw < GOOD_LIMIT) {
    quality   = AIR_GOOD;
    statusStr = "good";
    digitalWrite(LED_PIN, LOW);
  } else if (raw < WARN_LIMIT) {
    quality   = AIR_POOR;
    statusStr = "poor (warning)";
    digitalWrite(LED_PIN, HIGH);
  } else {
    quality   = AIR_DANGER;
    statusStr = "DANGER";
    digitalWrite(LED_PIN, HIGH);
  }

  Serial.print("avg: ");
  Serial.print(raw);
  Serial.print("  status: ");
  Serial.println(statusStr);

  delay(2000);
}
