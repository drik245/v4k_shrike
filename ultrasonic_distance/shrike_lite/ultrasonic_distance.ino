/*
 * Ultrasonic Distance - Shrike Lite (RP2040)
 * ==========================================
 * Measures distance with HC-SR04 and displays it on an SSD1306 OLED.
 * 
 * Dependencies:
 * - Adafruit SSD1306
 * - Adafruit GFX Library
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// RP2040 standard GPIO pins
#define TRIGGER_PIN 2
#define ECHO_PIN 3

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Shrike Lite (RP2040) Default I2C0
#define I2C_SDA 0
#define I2C_SCL 1

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
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
    int barWidth = map(distance, 0, 100, 0, 128);
    if (barWidth > 128) barWidth = 128;
    display.fillRect(0, 50, barWidth, 10, WHITE);
  }
  
  display.display();
  delay(500);
}
