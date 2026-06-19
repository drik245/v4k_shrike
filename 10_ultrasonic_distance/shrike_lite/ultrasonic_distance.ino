/*
 * Ultrasonic Distance – Shrike Lite (RP2040)
 * ==========================================
 * Board target : Raspberry Pi Pico (Generic RP2040)
 * Core         : arduino-pico (Earle Philhower)
 *
 * Measures distance with HC-SR04 and displays it on an SSD1306 OLED
 * (SPI) with a numeric readout and a graphical fill bar.
 *
 * Wiring (Shrike Lite header):
 *   HC-SR04 Trig → RP_IO26  (GPIO 26)
 *   HC-SR04 Echo → RP_IO27  (GPIO 27)
 *
 * OLED SPI wiring (Shrike Lite header – software SPI):
 *   MOSI → RP_IO7, CLK → RP_IO6, DC → RP_IO8,
 *   RST  → RP_IO9, CS  → RP_IO5
 *
 * Dependencies:
 *   Adafruit SSD1306, Adafruit GFX
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ── OLED SPI pins (Shrike Lite – RP2040, software SPI) ──
#define OLED_MOSI  7   // RP_IO7  – SPI0 TX
#define OLED_CLK   6   // RP_IO6  – SPI0 SCK
#define OLED_DC    8   // RP_IO8
#define OLED_RST   9   // RP_IO9
#define OLED_CS    5   // RP_IO5  – SPI0 CSn

// ── Ultrasonic Sensor Pins (Shrike Lite) ──
#define TRIGGER_PIN 26  // RP_IO26
#define ECHO_PIN    27  // RP_IO27

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

// ── Smoothing ──
#define NUM_READINGS 5
float readings[NUM_READINGS];
int   readIndex = 0;
bool  bufferFull = false;

float readDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1.0;
  return (duration / 2.0) / 29.1;
}

float getSmoothed(float newVal) {
  readings[readIndex] = newVal;
  readIndex = (readIndex + 1) % NUM_READINGS;
  if (readIndex == 0) bufferFull = true;

  int count = bufferFull ? NUM_READINGS : readIndex;
  float sum = 0;
  for (int i = 0; i < count; i++) sum += readings[i];
  return sum / count;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN,    INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(12, 20);
  display.println("HC-SR04");
  display.display();
  delay(1500);
}

void loop() {
  float raw = readDistance();

  display.clearDisplay();

  if (raw < 0) {
    display.setTextSize(1);
    display.setCursor(0, 8);
    display.print("Distance:");
    display.setTextSize(2);
    display.setCursor(4, 26);
    display.print("No echo");
    Serial.println("[SONAR] Out of range");
  } else {
    float dist = getSmoothed(raw);

    display.setTextSize(1);
    display.setCursor(0, 4);
    display.print("Distance:");

    display.setTextSize(3);
    display.setCursor(0, 18);
    display.print(dist, 1);
    display.setTextSize(1);
    display.print(" cm");

    // ── Graphical bar ──
    int barW = constrain(map((int)(dist * 10), 0, 2000, 0, 120), 0, 120);
    display.drawRoundRect(3, 50, 122, 10, 3, SSD1306_WHITE);
    display.fillRoundRect(4, 51, barW, 8, 2, SSD1306_WHITE);

    Serial.print("[SONAR] ");
    Serial.print(dist, 1);
    Serial.println(" cm");
  }

  display.display();
  delay(200);
}
