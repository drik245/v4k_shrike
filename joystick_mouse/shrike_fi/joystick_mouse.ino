/*
 * BLE Joystick Mouse – Shrike Fi (ESP32-S3)
 * ==========================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * This project uses Bluetooth LE to emulate a mouse.
 * Shrike Fi only – Shrike Lite (RP2040) has no Bluetooth.
 *
 * Joystick wiring (Shrike Fi header):
 *   X-axis  → ESP_IO1  (ADC1_CH0, GPIO 1)
 *   Y-axis  → ESP_IO2  (ADC1_CH1, GPIO 2)
 *   Switch  → ESP_IO3  (GPIO 3)
 *
 * Library: ESP32-BLE-Mouse
 *   https://github.com/T-vK/ESP32-BLE-Mouse
 */

#include <BleMouse.h>

// ── Joystick pins (Shrike Fi – ESP32-S3) ──
#define JOY_X_PIN   1    // ESP_IO1 – ADC1_CH0
#define JOY_Y_PIN   2    // ESP_IO2 – ADC1_CH1
#define JOY_SW_PIN  3    // ESP_IO3 – digital

// ── Tuning ──
#define DEAD_ZONE           80
#define MAX_MOUSE_SPEED     15
#define CALIBRATION_SAMPLES 64
#define CALIBRATION_DELAY   10
#define LOOP_DELAY_MS       10
#define DEBOUNCE_MS         50

BleMouse bleMouse("Vicharak Joystick", "Vicharak", 100);
int centerX = 2048;
int centerY = 2048;
bool lastButtonState  = HIGH;
unsigned long lastDebounceTime = 0;
bool currentButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  Serial.println("\n===== Vicharak BLE Joystick Mouse (Shrike Fi) =====");
  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  calibrateCenter();
  Serial.println("[BLE]  Starting BLE Mouse...");
  bleMouse.begin();
  Serial.println("[BLE]  Waiting for connection...");
}

void loop() {
  if (!bleMouse.isConnected()) {
    delay(500);
    return;
  }
  int rawX = analogRead(JOY_X_PIN);
  int rawY = analogRead(JOY_Y_PIN);
  int deltaX = rawX - centerX;
  int deltaY = rawY - centerY;
  if (abs(deltaX) < DEAD_ZONE) deltaX = 0;
  if (abs(deltaY) < DEAD_ZONE) deltaY = 0;
  int moveX = mapWithDeadZone(deltaX);
  int moveY = mapWithDeadZone(deltaY);
  bool reading = digitalRead(JOY_SW_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  lastButtonState = reading;

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    if (reading != currentButtonState) {
      currentButtonState = reading;
      if (currentButtonState == LOW) {
        bleMouse.press(MOUSE_LEFT);
        Serial.println("[BTN]  LEFT click pressed");
      } else {
        bleMouse.release(MOUSE_LEFT);
        Serial.println("[BTN]  LEFT click released");
      }
    }
  }
  if (moveX != 0 || moveY != 0) {
    bleMouse.move(moveX, moveY);
  }

  delay(LOOP_DELAY_MS);
}

void calibrateCenter() {
  Serial.println("[CAL]  Calibrating – keep joystick centered...");
  long sumX = 0;
  long sumY = 0;

  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    sumX += analogRead(JOY_X_PIN);
    sumY += analogRead(JOY_Y_PIN);
    delay(CALIBRATION_DELAY);
  }

  centerX = (int)(sumX / CALIBRATION_SAMPLES);
  centerY = (int)(sumY / CALIBRATION_SAMPLES);

  Serial.printf("[CAL]  Center X = %d,  Center Y = %d\n", centerX, centerY);
  Serial.println("[CAL]  Calibration complete.\n");
}

int mapWithDeadZone(int delta) {
  if (delta == 0) return 0;
  const int maxDelta = 2048 - DEAD_ZONE;
  int clamped = constrain(delta, -maxDelta, maxDelta);
  float normalized = (float)abs(clamped) / (float)maxDelta;
  float curved     = normalized * normalized;
  int   speed      = (int)(curved * MAX_MOUSE_SPEED);
  if (speed == 0) speed = 1;

  return (clamped > 0) ? speed : -speed;
}
