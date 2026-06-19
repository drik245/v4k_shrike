/*
  Alarm Clock - Shrike Fi (ESP32-S3)

  Offline alarm clock on a TM1637 display with three push buttons.
  Timekeeping is done with millis() off the 40 MHz crystal, no WiFi needed.
  Set time and alarm manually with buttons.

  MODE button cycles through: CLOCK > SET_HOUR > SET_MIN > SET_ALARM_HR > SET_ALARM_MIN > CLOCK
  UP button increments the value being set.
  DOWN button decrements the value being set.
  In clock mode, UP toggles alarm on/off.
  Any button dismisses a ringing alarm.

  Wiring:
    TM1637 CLK   - ESP_IO1 (GPIO 1)
    TM1637 DIO   - ESP_IO2 (GPIO 2)
    MODE button  - ESP_IO3 (GPIO 3, active low, internal pullup)
    UP button    - ESP_IO4 (GPIO 4, active low, internal pullup)
    DOWN button  - ESP_IO5 (GPIO 5, active low, internal pullup)
    Buzzer       - ESP_IO6 (GPIO 6)

  Needs: TM1637Display library (by Avishay Orpaz)
*/

#include <TM1637Display.h>

// pins
#define TM_CLK      1
#define TM_DIO      2
#define BTN_MODE    3
#define BTN_UP      4
#define BTN_DOWN    5
#define BUZZER_PIN  6

TM1637Display tm(TM_CLK, TM_DIO);

// clock
int hours   = 12;
int minutes = 0;
int seconds = 0;
unsigned long lastTick = 0;

// alarm
int  alarmHour    = 7;
int  alarmMinute  = 0;
bool alarmEnabled = false;
bool alarmRinging = false;
unsigned long alarmStartTime = 0;
const unsigned long ALARM_TIMEOUT = 60000; // auto dismiss after 60s

// modes
enum Mode { CLOCK, SET_HOUR, SET_MIN, SET_ALARM_HR, SET_ALARM_MIN };
Mode currentMode = CLOCK;

// button stuff
struct Button {
  int pin;
  int state;
  int lastReading;
  unsigned long lastDebounce;
  bool pressed;
};

Button btnMode = { BTN_MODE, HIGH, HIGH, 0, false };
Button btnUp   = { BTN_UP,   HIGH, HIGH, 0, false };
Button btnDown = { BTN_DOWN, HIGH, HIGH, 0, false };
const unsigned long DEBOUNCE_MS = 50;

// auto repeat when holding UP or DOWN in set modes
unsigned long upHeldSince     = 0;
unsigned long downHeldSince   = 0;
unsigned long lastAutoRepeatUp   = 0;
unsigned long lastAutoRepeatDown = 0;
const unsigned long REPEAT_DELAY = 600;
const unsigned long REPEAT_RATE  = 150;

// blink timer for set modes
unsigned long lastBlink = 0;
bool blinkOn = true;


void updateButton(Button &b) {
  int reading = digitalRead(b.pin);
  b.pressed = false;

  if (reading != b.lastReading) {
    b.lastDebounce = millis();
  }

  if ((millis() - b.lastDebounce) > DEBOUNCE_MS) {
    if (reading != b.state) {
      b.state = reading;
      if (b.state == LOW) {
        b.pressed = true;
      }
    }
  }
  b.lastReading = reading;
}

bool isHeld(Button &b) {
  return (b.state == LOW);
}


// keeps time using millis
void tickClock() {
  if (millis() - lastTick >= 1000) {
    lastTick += 1000;
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
      }
    }
  }
}


// show HH:MM on the display
void showTime(int h, int m, bool showColon) {
  int val = h * 100 + m;
  uint8_t dots = showColon ? 0b01000000 : 0x00;
  tm.showNumberDecEx(val, dots, true);
}

// blinks the digits being edited
void showSettingBlink(int h, int m, bool blinkLeft) {
  if (!blinkOn) {
    uint8_t segs[4];
    if (blinkLeft) {
      segs[0] = 0x00;
      segs[1] = 0x00;
      segs[2] = tm.encodeDigit(m / 10);
      segs[3] = tm.encodeDigit(m % 10);
    } else {
      segs[0] = tm.encodeDigit(h / 10);
      segs[1] = tm.encodeDigit(h % 10) | 0x80;
      segs[2] = 0x00;
      segs[3] = 0x00;
    }
    tm.setSegments(segs);
  } else {
    showTime(h, m, true);
  }
}


void checkAlarm() {
  if (alarmEnabled && !alarmRinging &&
      hours == alarmHour && minutes == alarmMinute && seconds == 0) {
    alarmRinging = true;
    alarmStartTime = millis();
    Serial.println("ALARM RINGING");
  }

  if (alarmRinging) {
    // beep beep pause
    unsigned long elapsed = (millis() - alarmStartTime) % 1000;
    if (elapsed < 150 || (elapsed > 250 && elapsed < 400)) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }

    if (millis() - alarmStartTime > ALARM_TIMEOUT) {
      alarmRinging = false;
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("alarm auto dismissed");
    }
  }
}

// wraps a value up within 0..max-1
int wrapUp(int val, int maxVal) {
  return (val + 1) % maxVal;
}

// wraps a value down within 0..max-1
int wrapDown(int val, int maxVal) {
  return (val - 1 + maxVal) % maxVal;
}


void setup() {
  Serial.begin(115200);

  pinMode(BTN_MODE,   INPUT_PULLUP);
  pinMode(BTN_UP,     INPUT_PULLUP);
  pinMode(BTN_DOWN,   INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  tm.setBrightness(5);

  // show "SEt" on boot
  uint8_t setMsg[] = {
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_D | SEG_E | SEG_F | SEG_G,
    0x00
  };
  tm.setSegments(setMsg);
  delay(1000);

  currentMode = SET_HOUR;
  lastTick = millis();

  Serial.println("alarm clock ready, set the time");
}


void loop() {
  updateButton(btnMode);
  updateButton(btnUp);
  updateButton(btnDown);
  tickClock();

  // blink toggle
  if (millis() - lastBlink >= 350) {
    lastBlink = millis();
    blinkOn = !blinkOn;
  }

  // any button dismisses alarm
  if (alarmRinging && (btnMode.pressed || btnUp.pressed || btnDown.pressed)) {
    alarmRinging = false;
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("alarm dismissed");
    return;
  }

  // auto repeat for UP when held
  bool upAction = btnUp.pressed;
  if (currentMode != CLOCK && isHeld(btnUp)) {
    if (btnUp.pressed) upHeldSince = millis();
    if (millis() - upHeldSince > REPEAT_DELAY) {
      if (millis() - lastAutoRepeatUp > REPEAT_RATE) {
        lastAutoRepeatUp = millis();
        upAction = true;
      }
    }
  }

  // auto repeat for DOWN when held
  bool downAction = btnDown.pressed;
  if (currentMode != CLOCK && isHeld(btnDown)) {
    if (btnDown.pressed) downHeldSince = millis();
    if (millis() - downHeldSince > REPEAT_DELAY) {
      if (millis() - lastAutoRepeatDown > REPEAT_RATE) {
        lastAutoRepeatDown = millis();
        downAction = true;
      }
    }
  }

  switch (currentMode) {

    case CLOCK:
      showTime(hours, minutes, (millis() / 500) % 2 == 0);
      checkAlarm();

      if (btnMode.pressed) {
        currentMode = SET_HOUR;
        Serial.println("set hour");
      }
      if (btnUp.pressed && !alarmRinging) {
        alarmEnabled = !alarmEnabled;
        Serial.print("alarm ");
        Serial.println(alarmEnabled ? "on" : "off");

        if (alarmEnabled) {
          showTime(alarmHour, alarmMinute, true);
        } else {
          uint8_t off[] = { SEG_G, SEG_G, SEG_G, SEG_G };
          tm.setSegments(off);
        }
        delay(800);
      }
      break;

    case SET_HOUR:
      showSettingBlink(hours, minutes, true);
      if (upAction)   hours = wrapUp(hours, 24);
      if (downAction)  hours = wrapDown(hours, 24);
      if (btnMode.pressed) {
        currentMode = SET_MIN;
        Serial.println("set min");
      }
      break;

    case SET_MIN:
      showSettingBlink(hours, minutes, false);
      if (upAction)   minutes = wrapUp(minutes, 60);
      if (downAction)  minutes = wrapDown(minutes, 60);
      if (btnMode.pressed) {
        currentMode = SET_ALARM_HR;
        Serial.print("time set to ");
        Serial.print(hours); Serial.print(":"); Serial.println(minutes);
        Serial.println("set alarm hour");
      }
      break;

    case SET_ALARM_HR:
      showSettingBlink(alarmHour, alarmMinute, true);
      if (upAction)   alarmHour = wrapUp(alarmHour, 24);
      if (downAction)  alarmHour = wrapDown(alarmHour, 24);
      if (btnMode.pressed) {
        currentMode = SET_ALARM_MIN;
        Serial.println("set alarm min");
      }
      break;

    case SET_ALARM_MIN:
      showSettingBlink(alarmHour, alarmMinute, false);
      if (upAction)   alarmMinute = wrapUp(alarmMinute, 60);
      if (downAction)  alarmMinute = wrapDown(alarmMinute, 60);
      if (btnMode.pressed) {
        alarmEnabled = true;
        currentMode = CLOCK;
        seconds = 0;
        lastTick = millis();
        Serial.print("alarm set to ");
        Serial.print(alarmHour); Serial.print(":"); Serial.println(alarmMinute);
        Serial.println("clock mode, alarm on");
      }
      break;
  }
}
