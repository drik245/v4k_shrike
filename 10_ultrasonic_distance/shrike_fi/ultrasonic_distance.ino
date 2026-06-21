/*
  Ultrasonic Distance - Shrike Fi (ESP32-S3)

  Measures distance with HC-SR04, shows it on an SSD1306 OLED (SPI)
  with a big number readout and a fill bar. Uses a 5-sample
  moving average for smoother readings.

  Wiring:
    HC-SR04 Trig - ESP_IO4 (GPIO 4)
    HC-SR04 Echo - ESP_IO5 (GPIO 5)
    OLED SPI: MOSI-35, CLK-36, DC-37, RST-38, CS-34

  Needs: Adafruit SSD1306, Adafruit GFX
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// oled spi pins
#define OLED_MOSI 35
#define OLED_CLK  36
#define OLED_DC   37
#define OLED_RST  38
#define OLED_CS   34

// ultrasonic pins
#define TRIGGER_PIN 4
#define ECHO_PIN    5

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

// smoothing
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
    Serial.println("out of range");
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

    // bar graph
    int barW = constrain(map((int)(dist * 10), 0, 2000, 0, 120), 0, 120);
    display.drawRoundRect(3, 50, 122, 10, 3, SSD1306_WHITE);
    display.fillRoundRect(4, 51, barW, 8, 2, SSD1306_WHITE);

    Serial.print(dist, 1);
    Serial.println(" cm");
  }

  display.display();
  delay(200);
}
