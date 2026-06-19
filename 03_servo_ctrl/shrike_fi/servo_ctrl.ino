/*
 * Servo Ctrl with OLED Gauge – Shrike Fi (ESP32-S3)
 * ===================================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Joystick-controlled servo with a real-time OLED gauge display.
 * Tilt the joystick X-axis to sweep the servo 0°–180°;
 * a needle-style gauge on the SSD1306 mirrors the angle live.
 *
 * Joystick wiring (Shrike Fi header):
 *   X-axis  → ESP_IO1  (ADC1_CH0, GPIO 1)
 *   Y-axis  → ESP_IO2  (ADC1_CH1, GPIO 2)   [read but unused]
 *   Switch  → ESP_IO3  (GPIO 3)              [unused]
 *
 * OLED SPI wiring (Shrike Fi header):
 *   MOSI/D1 → ESP_IO35  (GPIO 35)
 *   CLK/D0  → ESP_IO36  (GPIO 36)
 *   DC/MISO → ESP_IO37  (GPIO 37)
 *   RST/RES → ESP_IO38  (GPIO 38)
 *   CS      → ESP_IO34  (GPIO 34)
 *
 * Servo:
 *   Signal  → ESP_IO4   (GPIO 4)
 *
 * Libraries:
 *   Adafruit SSD1306, Adafruit GFX, ESP32Servo
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

#define S_WIDTH 128
#define S_HEIGHT 64

// ── OLED SPI pins (Shrike Fi – ESP32-S3) ──
#define OLED_MOSI 35   // ESP_IO35
#define OLED_CLK  36   // ESP_IO36
#define OLED_DC   37   // ESP_IO37
#define OLED_RST  38   // ESP_IO38
#define OLED_CS   34   // ESP_IO34

// ── Joystick pins (Shrike Fi – ESP32-S3) ──
#define X  1   // ESP_IO1 – ADC1_CH0
#define Y  2   // ESP_IO2 – ADC1_CH1
#define SW 3   // ESP_IO3 – digital

// ── Servo pin ──
#define SERVO_PIN 4   // ESP_IO4

#define LOOP_DELAY_MS       10
#define DEAD_ZONE           80
#define CALIBRATION_SAMPLES 64
#define CALIBRATION_DELAY   10

Adafruit_SSD1306 display(S_WIDTH, S_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);
Servo myservo;
int centerX, centerY;
int angle = 90;

void calibrate() {
  long sX = 0, sY = 0;
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    sX += analogRead(X);
    sY += analogRead(Y);
    delay(CALIBRATION_DELAY);
  }
  centerX = (int)(sX / CALIBRATION_SAMPLES);
  centerY = (int)(sY / CALIBRATION_SAMPLES);
}

void drawGauge(int ang) {
  display.clearDisplay();

  // ── Title ──
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 0);
  display.print("SERVO CONTROL");

  int cx = 64;
  int cy = 60;
  int r  = 30;

  // ── Arc ticks (every 45°) ──
  for (int t = 0; t <= 180; t += 45) {
    float rad = (float)(180 - t) * 3.14159 / 180.0;
    int x1 = cx + (int)((r - 3) * cos(rad));
    int y1 = cy - (int)((r - 3) * sin(rad));
    int x2 = cx + (int)((r + 2) * cos(rad));
    int y2 = cy - (int)((r + 2) * sin(rad));
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }

  // ── Arc outline ──
  for (int t = 0; t < 180; t += 3) {
    float r1 = (float)(180 - t) * 3.14159 / 180.0;
    float r2 = (float)(180 - t - 3) * 3.14159 / 180.0;
    int x1 = cx + (int)(r * cos(r1));
    int y1 = cy - (int)(r * sin(r1));
    int x2 = cx + (int)(r * cos(r2));
    int y2 = cy - (int)(r * sin(r2));
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }

  // ── Needle ──
  float needleRad = (float)(180 - ang) * 3.14159 / 180.0;
  int nx = cx + (int)((r - 6) * cos(needleRad));
  int ny = cy - (int)((r - 6) * sin(needleRad));
  display.drawLine(cx, cy, nx, ny, SSD1306_WHITE);
  display.fillCircle(cx, cy, 2, SSD1306_WHITE);

  // ── Angle readout ──
  display.setTextSize(2);
  display.setCursor(50, 12);
  if (ang < 100) display.print(" ");
  if (ang < 10)  display.print(" ");
  display.print(ang);
  display.setTextSize(1);
  display.print((char)247);   // degree symbol

  // ── Scale labels ──
  display.setTextSize(1);
  display.setCursor(10, 48);
  display.print("0");
  display.setCursor(105, 48);
  display.print("180");

  display.display();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nServo Ctrl with OLED Gauge (Shrike Fi)");

  pinMode(SW, INPUT_PULLUP);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // ── Servo init ──
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  myservo.setPeriodHertz(50);
  myservo.attach(SERVO_PIN, 500, 2400);
  myservo.write(90);
  Serial.println("[SERVO] Attached to GPIO " + String(SERVO_PIN));

  // ── OLED init ──
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("[OLED] SSD1306 init FAILED");
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 28);
  display.print("Calibrating...");
  display.display();

  calibrate();
  Serial.printf("[CAL] Center X=%d Y=%d\n", centerX, centerY);
}

void loop() {
  int rawX = analogRead(X);
  int dx = rawX - centerX;

  if (abs(dx) < DEAD_ZONE) {
    angle = 90;
  } else {
    angle = map(rawX, 0, 4095, 0, 180);
    angle = constrain(angle, 0, 180);
  }

  myservo.write(angle);
  drawGauge(angle);

  // ── Debug print every 500 ms ──
  static unsigned long lastDbg = 0;
  if (millis() - lastDbg > 500) {
    lastDbg = millis();
    Serial.printf("[SERVO] angle=%d  rawX=%d\n", angle, rawX);
  }

  delay(LOOP_DELAY_MS);
}
