/*
 * Ultrasonic Distance - Shrike Lite (RP2040)
 * ==========================================
 * Measures distance with HC-SR04 and displays it on an SSD1306 OLED.
 * 
 * Dependencies:
 * - Adafruit SSD1306
 * - Adafruit GFX Library
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
// Using valid RP_IO pins from the Shrike header
#define TRIGGER_PIN 26  // RP_IO26
#define ECHO_PIN    27  // RP_IO27

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("HC-SR04");
  display.display();
  delay(2000);
}

void loop() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("Distance:");

  if (duration == 0) {
    display.setTextSize(2);
    display.setCursor(0, 30);
    display.print("Out of Range");
    Serial.println("Out of Range");
  } else {
    float distance = (duration / 2.0) / 29.1;
    
    display.setTextSize(2);
    display.setCursor(0, 30);
    display.print(distance, 1);
    display.print(" cm");
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // Draw bar graph
    int barWidth = map(distance, 0, 200, 0, 128);
    if (barWidth > 128) barWidth = 128;
    display.fillRect(0, 50, barWidth, 10, SSD1306_WHITE);
  }
  
  display.display();
  delay(500);
}
