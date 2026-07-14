#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Shrike Fi Pins
#define I2C_SDA 7  // ESP_IO7
#define I2C_SCL 6  // ESP_IO6
#define LED_PIN 14 // ESP_IO14
#define BTN_PIN 5  // ESP_IO5

enum GameState { IDLE, WAITING, MEASURING, RESULT };
GameState state = IDLE;

unsigned long waitStartTime = 0;
unsigned long waitDuration = 0;
unsigned long ledTurnedOnTime = 0;
unsigned long lastBtnPress = 0;

void drawCenteredText(String text, int y, int size) {
  display.setTextSize(size);
  int textWidth = text.length() * 6 * size; 
  display.setCursor((SCREEN_WIDTH - textWidth) / 2, y);
  display.print(text);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(I2C_SDA, I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
  }
  display.setTextColor(WHITE);
  
  // Show Start Screen
  display.clearDisplay();
  drawCenteredText("REACTION TEST", 15, 1);
  drawCenteredText("Press to Start", 40, 1);
  display.display();
}

void loop() {
  bool btnPressed = (digitalRead(BTN_PIN) == LOW);
  
  if (btnPressed && millis() - lastBtnPress > 300) {
    lastBtnPress = millis();
    
    if (state == IDLE || state == RESULT) {
      state = WAITING;
      waitDuration = random(2000, 6000); 
      waitStartTime = millis();
      display.clearDisplay();
      drawCenteredText("Wait for LED...", 25, 1);
      display.display();
      digitalWrite(LED_PIN, LOW);
      
    } else if (state == WAITING) {
      state = RESULT;
      display.clearDisplay();
      drawCenteredText("TOO EARLY!", 25, 2);
      display.display();
      
    } else if (state == MEASURING) {
      unsigned long reactionTime = millis() - ledTurnedOnTime;
      state = RESULT;
      digitalWrite(LED_PIN, LOW);
      
      display.clearDisplay();
      drawCenteredText("Time:", 10, 2);
      drawCenteredText(String(reactionTime) + " ms", 40, 2);
      display.display();
    }
  }

  if (state == WAITING) {
    if (millis() - waitStartTime >= waitDuration) {
      state = MEASURING;
      ledTurnedOnTime = millis();
      digitalWrite(LED_PIN, HIGH);
      display.clearDisplay();
      drawCenteredText("PRESS NOW!", 25, 2);
      display.display();
    }
  }
}
