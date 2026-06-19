/*
 * Snake Game – Shrike Lite (RP2040)
 * ===================================
 * Board target : Raspberry Pi Pico (Generic RP2040)
 * Core         : arduino-pico (Earle Philhower)
 *
 * SSD1306 OLED (SPI) + Joystick snake game.
 * Works on Shrike Lite — no WiFi/BT required.
 *
 * Joystick wiring (Shrike Lite header):
 *   X-axis  → RP_IO26  (GPIO 26 – ADC0)
 *   Y-axis  → RP_IO27  (GPIO 27 – ADC1)
 *   Switch  → RP_IO28  (GPIO 28 – digital)
 *
 * OLED SPI wiring (Shrike Lite header – software SPI):
 *   MOSI → RP_IO7   (GPIO 7  – SPI0 TX)
 *   CLK  → RP_IO6   (GPIO 6  – SPI0 SCK)
 *   DC   → RP_IO8   (GPIO 8)
 *   RST  → RP_IO9   (GPIO 9)
 *   CS   → RP_IO5   (GPIO 5  – SPI0 CSn)
 *
 * Libraries:
 *   Adafruit SSD1306, Adafruit GFX, EEPROM (RP2040)
 *
 * Note: RP2040 ADC is 12-bit (0-4095) by default, same as ESP32.
 *       Preferences library is NOT available on RP2040 —
 *       EEPROM emulation via flash is used for high score storage.
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

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

#define LOOP_DELAY_MS 10
#define DEBOUNCE_MS 50
#define DEAD_ZONE 80
#define MAX_SPEED 15
#define CALIBRATION_SAMPLES 64
#define CALIBRATION_DELAY 10

#define G_SIZE 4
#define G_W 32
#define G_H 16
#define MAX_SNAKE 120

// ── EEPROM layout for high score ──
#define EEPROM_SIZE    8
#define HS_ADDR        0   // int stored at address 0
#define HS_MAGIC_ADDR  4   // magic value to detect first run
#define HS_MAGIC       0xBEEF

Adafruit_SSD1306 display(S_WIDTH, S_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

enum Dir       { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
enum GameState { STATE_PLAYING, STATE_PAUSED, STATE_GAMEOVER };
enum PauseItem { P_PLAY, P_SPEED, P_EXIT, P_COUNT };

const char* speedLabels[] = { "Slow", "Normal", "Fast", "Turbo" };
const int   speedDelays[] = { 220,    150,      100,    65 };
#define NUM_SPEEDS 4
int speedIdx = 1;

int snakeX[MAX_SNAKE];
int snakeY[MAX_SNAKE];
int snakeLen;
Dir dir, nextDir;

int foodX, foodY;
int score;
int highScore;
bool newBest;

GameState state;
PauseItem pauseSel;

int centerX = 2048;
int centerY = 2048;
unsigned long lastMoveTime;

bool btnPressed() {
  static bool lastStable = HIGH;

  bool cur = digitalRead(SW);
  if (cur == LOW && lastStable == HIGH) {
    delay(DEBOUNCE_MS);
    cur = digitalRead(SW);
    if (cur == LOW) {
      lastStable = LOW;
      return true;
    }
  }
  if (cur == HIGH) lastStable = HIGH;
  return false;
}

int readMenu() {
  static int lastD = 0;
  static unsigned long lastT = 0;

  int dx = analogRead(X) - centerX;
  int dy = analogRead(Y) - centerY;

  int d = 0;
  if (abs(dx) > DEAD_ZONE || abs(dy) > DEAD_ZONE) {
    if (abs(dx) > abs(dy))
      d = (dx > 0) ? 4 : 3;
    else
      d = (dy > 0) ? 2 : 1;
  }

  if (d == 0) { lastD = 0; return 0; }
  if (d != lastD || (millis() - lastT > 280)) {
    lastD = d;
    lastT = millis();
    return d;
  }
  return 0;
}

void readGame() {
  int dx = analogRead(X) - centerX;
  int dy = analogRead(Y) - centerY;

  if (abs(dx) > DEAD_ZONE || abs(dy) > DEAD_ZONE) {
    if (abs(dx) > abs(dy)) {
      if (dx > 0 && dir != DIR_LEFT)  nextDir = DIR_RIGHT;
      if (dx < 0 && dir != DIR_RIGHT) nextDir = DIR_LEFT;
    } else {
      if (dy > 0 && dir != DIR_UP)    nextDir = DIR_DOWN;
      if (dy < 0 && dir != DIR_DOWN)  nextDir = DIR_UP;
    }
  }
}

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

void loadHS() {
  EEPROM.begin(EEPROM_SIZE);
  uint16_t magic;
  EEPROM.get(HS_MAGIC_ADDR, magic);
  if (magic == HS_MAGIC) {
    EEPROM.get(HS_ADDR, highScore);
  } else {
    // first run — initialise
    highScore = 0;
    EEPROM.put(HS_ADDR, highScore);
    EEPROM.put(HS_MAGIC_ADDR, (uint16_t)HS_MAGIC);
    EEPROM.commit();
  }
}

void savehighScore() {
  if (score > highScore) {
    highScore = score;
    newBest = true;
    EEPROM.put(HS_ADDR, highScore);
    EEPROM.commit();
  }
}

void placeFood() {
  bool valid;
  do {
    valid = true;
    foodX = random(1, G_W - 1);
    foodY = random(1, G_H - 1);
    for (int i = 0; i < snakeLen; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) {
        valid = false;
        break;
      }
    }
  } while (!valid);
}

void resetGame() {
  snakeLen = 3;
  dir      = DIR_RIGHT;
  nextDir  = DIR_RIGHT;
  score    = 0;
  newBest  = false;
  state    = STATE_PLAYING;
  pauseSel = P_PLAY;

  for (int i = 0; i < snakeLen; i++) {
    snakeX[i] = G_W / 2 - i;
    snakeY[i] = G_H / 2;
  }

  placeFood();
  lastMoveTime = millis();
}

void moveSnake() {
  dir = nextDir;

  for (int i = snakeLen - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  switch (dir) {
    case DIR_UP:    snakeY[0]--; break;
    case DIR_DOWN:  snakeY[0]++; break;
    case DIR_LEFT:  snakeX[0]--; break;
    case DIR_RIGHT: snakeX[0]++; break;
  }

  // wall collision
  if (snakeX[0] < 0 || snakeX[0] >= G_W ||
      snakeY[0] < 0 || snakeY[0] >= G_H) {
    state = STATE_GAMEOVER;
    savehighScore();
    return;
  }

  // self collision
  for (int i = 1; i < snakeLen; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      state = STATE_GAMEOVER;
      savehighScore();
      return;
    }
  }

  // eat food
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    if (snakeLen < MAX_SNAKE) snakeLen++;
    score++;
    placeFood();
  }
}

void drawFood() {
  int cx = foodX * G_SIZE + G_SIZE / 2;
  int cy = foodY * G_SIZE + G_SIZE / 2;

  int frame = (millis() / 120) % 6;
  const int radii[] = { 1, 2, 3, 3, 2, 1 };
  int r = radii[frame];
  display.fillCircle(cx, cy, 1, SSD1306_WHITE);
  // outer ring
  if (r > 1) display.drawCircle(cx, cy, r, SSD1306_WHITE);
}

void drawGame() {
  display.clearDisplay();

  // border
  display.drawRect(0, 0, S_WIDTH, S_HEIGHT, SSD1306_WHITE);

  // food orb
  drawFood();

  // snake
  for (int i = 0; i < snakeLen; i++) {
    int px = snakeX[i] * G_SIZE;
    int py = snakeY[i] * G_SIZE;
    if (i == 0) {
      // head filled
      display.fillRoundRect(px, py, G_SIZE, G_SIZE, 1, SSD1306_WHITE);
    } else {
      // body outlined
      display.drawRoundRect(px, py, G_SIZE, G_SIZE, 1, SSD1306_WHITE);
    }
  }

  // score overlay
  display.fillRect(1, 1, 20, 9, SSD1306_BLACK);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print(score);

  display.display();
}

void drawPauseMenu() {
  display.clearDisplay();

  // title
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 2);
  display.print("PAUSED");

  // divider
  display.drawLine(10, 20, 118, 20, SSD1306_WHITE);

  // menu items
  const char* labels[] = { "Resume", "Speed", "Exit" };
  int yStart = 25;
  int lineH  = 13;
  display.setTextSize(1);

  for (int i = 0; i < P_COUNT; i++) {
    int yy = yStart + i * lineH;

    if (i == (int)pauseSel) {
      // highlighted row
      display.fillRoundRect(8, yy - 1, 112, 11, 2, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(14, yy);
    display.print(labels[i]);

    if (i == P_SPEED) {
      display.print(": <");
      display.print(speedLabels[speedIdx]);
      display.print(">");
    }

    display.setTextColor(SSD1306_WHITE);
  }

  display.display();
}

void drawGameOver() {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 2);
  display.print("GAME OVER");

  display.setTextSize(1);
  display.setCursor(30, 25);
  display.print("Score: ");
  display.print(score);

  display.setCursor(30, 37);
  display.print("Best:  ");
  display.print(highScore);

  // new high score
  if (newBest) {
    if ((millis() / 400) % 2 == 0) {
      display.setCursor(20, 48);
      display.print("** NEW BEST! **");
    }
  }

  // restart
  if ((millis() / 500) % 2 == 0) {
    display.setCursor(12, 56);
    display.print("Press to restart");
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nSnake Game (Shrike Lite)");

  pinMode(SW, INPUT_PULLUP);
  analogReadResolution(12);
  // Note: RP2040 does not need analogSetAttenuation()

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("[OLED] SSD1306 init FAILED");
    for (;;);
  }

  // splash
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 10);
  display.print("SNAKE!");
  display.setTextSize(1);
  display.setCursor(16, 40);
  display.print("Calibrating...");
  display.display();

  calibrate();
  Serial.printf("[CAL]  Center X=%d  Y=%d\n", centerX, centerY);

  loadHS();
  Serial.printf("[HS]   High Score: %d\n", highScore);

  randomSeed(analogRead(0));
  resetGame();
}

void loop() {
  bool btn = btnPressed();

  switch (state) {
    case STATE_PLAYING: {
      if (btn) {
        state = STATE_PAUSED;
        pauseSel = P_PLAY;
        break;
      }

      readGame();

      if (millis() - lastMoveTime >= (unsigned long)speedDelays[speedIdx]) {
        moveSnake();
        lastMoveTime = millis();
      }

      drawGame();
      break;
    }

    case STATE_PAUSED: {
      int joy = readMenu();

      // up / down to navigate items
      if (joy == 1 && pauseSel > 0)
        pauseSel = (PauseItem)(pauseSel - 1);
      if (joy == 2 && pauseSel < P_COUNT - 1)
        pauseSel = (PauseItem)(pauseSel + 1);

      // left / right adjusts speed when Speed row is selected
      if (pauseSel == P_SPEED) {
        if (joy == 3 && speedIdx > 0)              speedIdx--;
        if (joy == 4 && speedIdx < NUM_SPEEDS - 1) speedIdx++;
      }

      // button selects current item
      if (btn) {
        switch (pauseSel) {
          case P_PLAY:
            state = STATE_PLAYING;
            lastMoveTime = millis();
            break;
          case P_SPEED:
            speedIdx = (speedIdx + 1) % NUM_SPEEDS;
            break;
          case P_EXIT:
            state = STATE_GAMEOVER;
            savehighScore();
            break;
          default: break;
        }
      }

      drawPauseMenu();
      break;
    }

    case STATE_GAMEOVER: {
      drawGameOver();
      if (btn) {
        delay(200);
        resetGame();
      }
      break;
    }
  }

  delay(LOOP_DELAY_MS);
}
