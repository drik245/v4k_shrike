/*
  Project 34: BLE Xbox-Style Gamepad (Shrike Fi)

  Turns the Shrike Fi into a wireless BLE HID gamepad with 2 thumbsticks
  and 4 face buttons. Shows up as "Shrike Controller" on your PC.

  Library needed: "ESP32 BLE Gamepad" by lemmingDev
  Install via: Sketch > Include Library > Manage Libraries

  Wiring:
    Left stick VRX  -> ESP_IO4
    Left stick VRY  -> ESP_IO5
    Right stick VRX -> ESP_IO6
    Right stick VRY -> ESP_IO7
    Button A        -> ESP_IO14 to GND
    Button B        -> ESP_IO15 to GND
    Button X        -> ESP_IO16 to GND
    Button Y        -> ESP_IO17 to GND

  Joystick VCC -> 3.3V, GND -> GND.
  Buttons use INPUT_PULLUP so no external resistors needed.
*/

#include <BleGamepad.h>
#include <Arduino.h>

// --- CONFIGURATION ---
// Change these to 'true' if any axis moves in the wrong direction!
#define INVERT_LEFT_X  true
#define INVERT_LEFT_Y  true
#define INVERT_RIGHT_X false
#define INVERT_RIGHT_Y false

// Change these to 'true' if up/down is swapped with left/right
#define SWAP_LEFT_AXES false
#define SWAP_RIGHT_AXES false

#define DEADZONE 150
// ---------------------

// Joystick pins (all ADC-capable)
#define L_STICK_X  4   // ESP_IO4
#define L_STICK_Y  5   // ESP_IO5
#define R_STICK_X  6   // ESP_IO6
#define R_STICK_Y  7   // ESP_IO7

// Button pins (active LOW)
#define BTN_A  14  // ESP_IO14
#define BTN_B  15  // ESP_IO15
#define BTN_X  16  // ESP_IO16
#define BTN_Y  17  // ESP_IO17
#define BTN_L3 2   // ESP_IO2 (Left Stick Button)
#define BTN_R3 3   // ESP_IO3 (Right Stick Button)

BleGamepad bleGamepad("Shrike Controller", "Vicharak", 100);

struct AxisData {
  int center = 2048;
  int min_val = 2048 - 50;
  int max_val = 2048 + 50;
};

AxisData lx_axis, ly_axis, rx_axis, ry_axis;

// Average 8 ADC samples to smooth out hardware noise
int readStick(int pin) {
  int total = 0;
  for (int i = 0; i < 8; i++) {
    total += analogRead(pin);
  }
  return total / 8;
}

// Dynamically auto-calibrating mapping function
int16_t mapStick(int raw, AxisData &axis, bool invert) {
  // Auto-expand bounds if we move past known limits
  if (raw < axis.min_val) axis.min_val = raw;
  if (raw > axis.max_val) axis.max_val = raw;

  int delta = raw - axis.center;
  if (abs(delta) < DEADZONE) return 0;
  
  float mapped;
  if (delta < 0) {
    mapped = (float)(delta + DEADZONE) / max(1, (axis.center - axis.min_val - DEADZONE));
  } else {
    mapped = (float)(delta - DEADZONE) / max(1, (axis.max_val - axis.center - DEADZONE));
  }
  
  int16_t val = (int16_t)constrain(mapped * 32767.0f, -32767, 32767);
  return invert ? -val : val;
}

void setup() {
  Serial.begin(115200);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_X, INPUT_PULLUP);
  pinMode(BTN_Y, INPUT_PULLUP);
  pinMode(BTN_L3, INPUT_PULLUP);
  pinMode(BTN_R3, INPUT_PULLUP);

  // Auto-calibration: Sample the initial position to establish centers
  // Do NOT touch the sticks while booting!
  delay(100); // Let ADC settle
  long lx_sum = 0, ly_sum = 0, rx_sum = 0, ry_sum = 0;
  for (int i = 0; i < 20; i++) {
    lx_sum += readStick(L_STICK_X);
    ly_sum += readStick(L_STICK_Y);
    rx_sum += readStick(R_STICK_X);
    ry_sum += readStick(R_STICK_Y);
    delay(10);
  }
  lx_axis.center = lx_sum / 20;
  ly_axis.center = ly_sum / 20;
  rx_axis.center = rx_sum / 20;
  ry_axis.center = ry_sum / 20;

  // Initialize bounds slightly around center to prevent divide-by-zero
  lx_axis.min_val = lx_axis.center - 50; lx_axis.max_val = lx_axis.center + 50;
  ly_axis.min_val = ly_axis.center - 50; ly_axis.max_val = ly_axis.center + 50;
  rx_axis.min_val = rx_axis.center - 50; rx_axis.max_val = rx_axis.center + 50;
  ry_axis.min_val = ry_axis.center - 50; ry_axis.max_val = ry_axis.center + 50;

  BleGamepadConfiguration config;
  config.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  config.setAutoReport(false);
  config.setButtonCount(16);
  config.setWhichAxes(true, true, true, true, false, false, false, false);

  bleGamepad.begin(&config);
  Serial.println("Shrike Controller is ready. Do a full circle with the sticks to set max limits!");
}

void loop() {
  if (!bleGamepad.isConnected()) {
    delay(500);
    return;
  }

  // Read and auto-calibrate sticks
  int16_t lx = mapStick(readStick(L_STICK_X), lx_axis, INVERT_LEFT_X);
  int16_t ly = mapStick(readStick(L_STICK_Y), ly_axis, INVERT_LEFT_Y);
  if (SWAP_LEFT_AXES) { int16_t temp = lx; lx = ly; ly = temp; }

  int16_t rx = mapStick(readStick(R_STICK_X), rx_axis, INVERT_RIGHT_X);
  int16_t ry = mapStick(readStick(R_STICK_Y), ry_axis, INVERT_RIGHT_Y);
  if (SWAP_RIGHT_AXES) { int16_t temp = rx; rx = ry; ry = temp; }

  // Send right stick data to Z/Rz AND Rx/Ry to guarantee it gets picked up
  // setAxes(x, y, z, rZ, rX, rY, slider1, slider2)
  bleGamepad.setAxes(lx, ly, rx, ry, rx, ry, 0, 0);

  if (digitalRead(BTN_A) == LOW) bleGamepad.press(BUTTON_1); else bleGamepad.release(BUTTON_1);
  if (digitalRead(BTN_B) == LOW) bleGamepad.press(BUTTON_2); else bleGamepad.release(BUTTON_2);
  if (digitalRead(BTN_X) == LOW) bleGamepad.press(BUTTON_3); else bleGamepad.release(BUTTON_3);
  if (digitalRead(BTN_Y) == LOW) bleGamepad.press(BUTTON_4); else bleGamepad.release(BUTTON_4);
  
  if (digitalRead(BTN_L3) == LOW) bleGamepad.press(BUTTON_11); else bleGamepad.release(BUTTON_11);
  if (digitalRead(BTN_R3) == LOW) bleGamepad.press(BUTTON_12); else bleGamepad.release(BUTTON_12);

  bleGamepad.sendReport();
  delay(15);
}
