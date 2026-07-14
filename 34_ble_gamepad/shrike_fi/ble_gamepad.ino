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

BleGamepad bleGamepad("Shrike Controller", "Vicharak", 100);

// Deadzone avoids stick drift when the joystick is centered
const int   DEADZONE   = 150;
const float ADC_CENTER = 2047.5f;
const float ADC_RANGE  = 2047.5f;

// Map raw ADC (0-4095) to int16 range (-32767 to 32767) with deadzone
int16_t mapStick(int raw) {
  float delta = (float)(raw - ADC_CENTER);
  if (abs(delta) < DEADZONE) delta = 0.0f;
  return (int16_t)constrain((delta / ADC_RANGE) * 32767.0f, -32767, 32767);
}

void setup() {
  Serial.begin(115200);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_X, INPUT_PULLUP);
  pinMode(BTN_Y, INPUT_PULLUP);

  BleGamepadConfiguration config;
  config.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  config.setAutoReport(false);   // we batch all updates into one sendReport() call
  config.setButtonCount(16);
  config.setWhichAxes(true, true, true, true, false, false, false, false);

  bleGamepad.begin(&config);
  Serial.println("Shrike Controller is advertising. Open Bluetooth on your PC to pair.");
}

void loop() {
  if (!bleGamepad.isConnected()) return;

  // Read both sticks and send as HID axes
  bleGamepad.setAxes(
    mapStick(analogRead(L_STICK_X)),
    mapStick(analogRead(L_STICK_Y)),
    0, 0,
    mapStick(analogRead(R_STICK_X)),
    mapStick(analogRead(R_STICK_Y)),
    0, 0
  );

  // Face buttons — pressed = LOW because of INPUT_PULLUP
  if (digitalRead(BTN_A) == LOW) bleGamepad.press(BUTTON_1);
  else bleGamepad.release(BUTTON_1);

  if (digitalRead(BTN_B) == LOW) bleGamepad.press(BUTTON_2);
  else bleGamepad.release(BUTTON_2);

  if (digitalRead(BTN_X) == LOW) bleGamepad.press(BUTTON_3);
  else bleGamepad.release(BUTTON_3);

  if (digitalRead(BTN_Y) == LOW) bleGamepad.press(BUTTON_4);
  else bleGamepad.release(BUTTON_4);

  bleGamepad.sendReport();
  delay(15); // ~66 Hz
}
