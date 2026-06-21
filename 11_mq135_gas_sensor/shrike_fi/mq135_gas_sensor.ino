/*
  MQ-135 Gas Sensor - Shrike Fi (ESP32-S3)

  Reads analog output from MQ-135 with 16-sample averaging
  for stable readings. LED warns on poor air, buzzer on danger.

  Wiring:
    MQ-135 AO   - ESP_IO1 (GPIO 1, ADC1_CH0)
    Warning LED - ESP_IO21 (GPIO 21)
    Buzzer      - ESP_IO3 (GPIO 3, optional)

  No external libraries needed.
*/

// pins
#define MQ135_PIN   1
#define LED_PIN     21
#define BUZZER_PIN  3

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

  analogSetPinAttenuation(MQ135_PIN, ADC_11db);

  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN,    LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("starting MQ-135 sensor...");
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
    digitalWrite(LED_PIN,    LOW);
    digitalWrite(BUZZER_PIN, LOW);
  } else if (raw < WARN_LIMIT) {
    quality   = AIR_POOR;
    statusStr = "poor (warning)";
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    quality   = AIR_DANGER;
    statusStr = "DANGER";
    digitalWrite(LED_PIN,    HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  }

  Serial.print("avg: ");
  Serial.print(raw);
  Serial.print("  status: ");
  Serial.println(statusStr);

  delay(2000);
}
