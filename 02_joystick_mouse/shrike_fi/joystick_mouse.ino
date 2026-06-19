/*
 * BLE Joystick Mouse – Shrike Fi (ESP32-S3)
 * ==========================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * Bluetooth LE HID mouse driven by an analog joystick.
 *
 * Joystick wiring (Shrike Fi header):
 *   X-axis  → ESP_IO1  (ADC1_CH0, GPIO 1)
 *   Y-axis  → ESP_IO2  (ADC1_CH1, GPIO 2)
 *   Switch  → ESP_IO3  (GPIO 3)
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEHIDDevice.h>
#include <HIDTypes.h>

// ── Joystick pins (Shrike Fi – ESP32-S3) ──
#define JOY_X_PIN   1    // ESP_IO1 – ADC1_CH0
#define JOY_Y_PIN   2    // ESP_IO2 – ADC1_CH1
#define JOY_SW_PIN  3    // ESP_IO3 – digital

// ── Built-in LED (Shrike Fi – ESP_IO21) ──
#define LED_PIN 21   // ESP_IO21 – onboard LED

// ── Tuning ──
#define DEAD_ZONE           80
#define MAX_MOUSE_SPEED     15
#define CALIBRATION_SAMPLES 64
#define CALIBRATION_DELAY   10
#define LOOP_DELAY_MS       10
#define DEBOUNCE_MS         50

// ── HID Mouse Report Descriptor ──
static const uint8_t mouseHidDescriptor[] = {
  0x05, 0x01,  // Usage Page (Generic Desktop)
  0x09, 0x02,  // Usage (Mouse)
  0xA1, 0x01,  // Collection (Application)
  0x09, 0x01,  //   Usage (Pointer)
  0xA1, 0x00,  //   Collection (Physical)

  // 3 buttons
  0x05, 0x09,  //     Usage Page (Buttons)
  0x19, 0x01,  //     Usage Minimum (Button 1)
  0x29, 0x03,  //     Usage Maximum (Button 3)
  0x15, 0x00,  //     Logical Minimum (0)
  0x25, 0x01,  //     Logical Maximum (1)
  0x95, 0x03,  //     Report Count (3)
  0x75, 0x01,  //     Report Size (1 bit)
  0x81, 0x02,  //     Input (Data, Variable, Absolute)

  // 5-bit padding
  0x95, 0x01,  //     Report Count (1)
  0x75, 0x05,  //     Report Size (5 bits)
  0x81, 0x01,  //     Input (Constant) — padding

  // X, Y, Wheel  (relative, -127..+127)
  0x05, 0x01,  //     Usage Page (Generic Desktop)
  0x09, 0x30,  //     Usage (X)
  0x09, 0x31,  //     Usage (Y)
  0x09, 0x38,  //     Usage (Wheel)
  0x15, 0x81,  //     Logical Minimum (-127)
  0x25, 0x7F,  //     Logical Maximum (+127)
  0x75, 0x08,  //     Report Size (8 bits)
  0x95, 0x03,  //     Report Count (3)
  0x81, 0x06,  //     Input (Data, Variable, Relative)

  0xC0,        //   End Collection (Physical)
  0xC0         // End Collection (Application)
};

// ── BLE objects ──
BLEHIDDevice* hid       = nullptr;
BLECharacteristic* input = nullptr;
bool bleConnected = false;
BLEServer* pServer = nullptr;

// ── Connection callbacks ──
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* server) override {
    bleConnected = true;
    digitalWrite(LED_PIN, HIGH);   // LED on
    Serial.println("[BLE]  ✓ Device connected!");
  }
  void onDisconnect(BLEServer* server) override {
    bleConnected = false;
    digitalWrite(LED_PIN, LOW);    // LED off
    Serial.println("[BLE]  ✗ Disconnected — re-advertising...");
    BLEDevice::startAdvertising();
  }
};

// ── Joystick state ──
int centerX = 2048;
int centerY = 2048;
bool lastButtonState    = HIGH;
bool currentButtonState = HIGH;
unsigned long lastDebounceTime = 0;
uint8_t buttons = 0;

// ── Forward declarations ──
void initBLE();
void sendMouseReport(uint8_t btn, int8_t x, int8_t y);
void calibrateCenter();
int  mapWithDeadZone(int delta);

void setup() {
  Serial.begin(115200);
  Serial.println("\n===== Vicharak BLE Joystick Mouse (Shrike Fi) =====");

  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  calibrateCenter();
  initBLE();
}

void initBLE() {
  Serial.println("[BLE]  Initializing BLE HID Mouse...");

  BLEDevice::init("Vicharak Joystick");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  hid = new BLEHIDDevice(pServer);
  input = hid->inputReport(0);          // report ID 0

  hid->manufacturer()->setValue("Vicharak");
  hid->pnp(0x02, 0xE502, 0xA111, 0x0210);
  hid->hidInfo(0x00, 0x01);             // flags, country code
  hid->reportMap((uint8_t*)mouseHidDescriptor, sizeof(mouseHidDescriptor));
  hid->startServices();

  // BLE security — required for HID bonding
  BLESecurity* pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  // Start advertising with HID appearance
  BLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_MOUSE);
  pAdvertising->addServiceUUID(hid->hidService()->getUUID());
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);   // helps iPhone find the device
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();

  Serial.println("[BLE]  Advertising — pair with \"Vicharak Joystick\"");
}

void loop() {
  if (!bleConnected) {
    // Slow blink while advertising
    static bool ledOn = false;
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
    delay(500);
    return;
  }

  // ── Read joystick ──
  int rawX = analogRead(JOY_X_PIN);
  int rawY = analogRead(JOY_Y_PIN);

  int deltaX = rawX - centerX;
  int deltaY = rawY - centerY;
  if (abs(deltaX) < DEAD_ZONE) deltaX = 0;
  if (abs(deltaY) < DEAD_ZONE) deltaY = 0;

  int moveX = mapWithDeadZone(deltaX);
  int moveY = mapWithDeadZone(deltaY);

  // ── Button debounce ──
  bool reading = digitalRead(JOY_SW_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  lastButtonState = reading;

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    if (reading != currentButtonState) {
      currentButtonState = reading;
      if (currentButtonState == LOW) {
        buttons = 0x01;                  // left button pressed
        Serial.println("[BTN]  LEFT click pressed");
      } else {
        buttons = 0x00;
        Serial.println("[BTN]  LEFT click released");
      }
      sendMouseReport(buttons, 0, 0);    // button-only report
    }
  }

  // ── Cursor movement ──
  if (moveX != 0 || moveY != 0) {
    sendMouseReport(buttons, moveX, moveY);
  }

  delay(LOOP_DELAY_MS);
}

void sendMouseReport(uint8_t btn, int8_t x, int8_t y) {
  uint8_t report[4] = { btn, (uint8_t)x, (uint8_t)y, 0x00 };
  input->setValue(report, sizeof(report));
  input->notify();
}

void calibrateCenter() {
  Serial.println("[CAL]  Calibrating – keep joystick centered...");
  long sumX = 0, sumY = 0;

  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    sumX += analogRead(JOY_X_PIN);
    sumY += analogRead(JOY_Y_PIN);
    delay(CALIBRATION_DELAY);
  }

  centerX = (int)(sumX / CALIBRATION_SAMPLES);
  centerY = (int)(sumY / CALIBRATION_SAMPLES);

  Serial.printf("[CAL]  Center X = %d,  Center Y = %d\n", centerX, centerY);
  Serial.println("[CAL]  Calibration complete.\n");
}

int mapWithDeadZone(int delta) {
  if (delta == 0) return 0;
  const int maxDelta = 2048 - DEAD_ZONE;
  int clamped = constrain(delta, -maxDelta, maxDelta);
  float normalized = (float)abs(clamped) / (float)maxDelta;
  float curved     = normalized * normalized;
  int   speed      = (int)(curved * MAX_MOUSE_SPEED);
  if (speed == 0) speed = 1;
  return (clamped > 0) ? speed : -speed;
}
