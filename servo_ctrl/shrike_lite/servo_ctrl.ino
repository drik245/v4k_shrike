/*
 * Servo Ctrl with OLED Gauge – Shrike Lite (RP2040)
 * ===================================================
 * Board target : Raspberry Pi Pico (Generic RP2040)
 * Core         : arduino-pico (Earle Philhower)
 *
 * Joystick-controlled servo with a real-time OLED gauge display.
 * Tilt the joystick X-axis to sweep the servo 0°–180°;
 * a needle-style gauge on the SSD1306 mirrors the angle live.
 *
 * Joystick wiring (Shrike Lite header):
 *   X-axis  → RP_IO26  (GPIO 26 – ADC0)
 *   Y-axis  → RP_IO27  (GPIO 27 – ADC1)   [read but unused]
 *   Switch  → RP_IO28  (GPIO 28 – digital) [unused]
 *
 * OLED SPI wiring (Shrike Lite header – software SPI):
 *   MOSI → RP_IO7   (GPIO 7  – SPI0 TX)
 *   CLK  → RP_IO6   (GPIO 6  – SPI0 SCK)
 *   DC   → RP_IO8   (GPIO 8)
 *   RST  → RP_IO9   (GPIO 9)
 *   CS   → RP_IO5   (GPIO 5  – SPI0 CSn)
 *
 * Servo:
 *   Signal  → RP_IO10  (GPIO 10)
 *
 * Libraries:
 *   Adafruit SSD1306, Adafruit GFX, Servo (built-in)
 *
 * Note: RP2040 ADC is 12-bit (0-4095) by default, same as ESP32.
 *       The arduino-pico core includes a built-in Servo library.
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define S_WIDTH 128
#define S_HEIGHT 64

// ── OLED SPI pins (Shrike Lite – RP2040, software SPI) ──
#define OLED_MOSI  7   // RP_IO7  – SPI0 TX
#define OLED_CLK   6   // RP_IO6  – SPI0 SCK
#define OLED_DC    8   // RP_IO8
#define OLED_RST   9   // RP_IO9
#define OLED_CS    5   // RP_IO5  – SPI0 CSn

// ── Joystick pins (Shrike Lite – RP2040) ──
#define X  26  // RP_IO26 – ADC0
#define Y  27  // RP_IO27 – ADC1
#define SW 28  // RP_IO28 – digital

// ── Servo pin ──
#define SERVO_PIN 10   // RP_IO10

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
  Serial.println("\nServo Ctrl with OLED Gauge (Shrike Lite)");

  pinMode(SW, INPUT_PULLUP);
  analogReadResolution(12);
  // Note: RP2040 does not need analogSetAttenuation()

  // ── Servo init ──
  myservo.attach(SERVO_PIN);
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
