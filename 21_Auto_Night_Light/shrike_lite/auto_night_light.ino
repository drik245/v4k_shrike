#define ldr_pin 26
#define led 15

const long readInterval = 800;
unsigned long lastRead = 0;
const int threshold = 230;

int rawValue = 0; 
int brightness = 0;
unsigned long lastStep = 0;

const unsigned long timeout = 10000;   // 10s in milliseconds
unsigned long darkSince = 0;
bool wasDark = false;

// Breath state machine
enum BreathState { GLOWING, HOLDING, DIMMING };
BreathState breathState = GLOWING;
unsigned long holdStart = 0;

void dim(int fadeOut) {                 //fade-out function
  unsigned long now = micros();
  if (now - lastStep >= fadeOut) {
    lastStep = now;
    if (brightness > 0) brightness--;
    analogWrite(led, brightness);
  }
}

void glow(int fadeIn) {                 //fade-in function
  unsigned long now = micros();
  if (now - lastStep >= fadeIn) {
    lastStep = now;
    if (brightness < 4095) brightness++;
    analogWrite(led, brightness);
  }
}

void breath(int breathInterval) {       //breathing function
  switch (breathState) {
    case GLOWING:                       //breathing state
      glow(500);
      if (brightness >= 4095) {
        breathState = HOLDING;
        holdStart = micros();
      }
      break;
    case HOLDING:                       //holding state
      if (micros() - holdStart >= breathInterval) {
        breathState = DIMMING;
      }
      break;
    case DIMMING:                       //dimming state
      dim(500);
      if (brightness <= 0) {
        breathState = GLOWING;
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  analogWriteResolution(12);            // set PWM resolution to 12-bit (0-4095)
  analogReadResolution(8);              // set ADC resolution to 8-bit (0-255)
  analogWrite(led, 0);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastRead >= readInterval) {
    lastRead = currentMillis;
    rawValue = analogRead(ldr_pin);
    Serial.println(rawValue);
  }

  bool isDark = (rawValue > threshold);

  if (!isDark) {
    wasDark = false;                     // Lights ON, reset dark-timer
    darkSince = 0;
    dim(500);                            // off in lit enviornment
  } else {
    if (!wasDark) {
      wasDark = true;
      darkSince = currentMillis;          // mark when it just went dark
    }
    if (currentMillis - darkSince < timeout) {
      glow(500);                   
    } else {
      breath(20000);                       //normal breathing after solid-on in the dark 
    }
  }
}