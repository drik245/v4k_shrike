#define ldr_pin 26
#define led 15
const long readInterval = 800;
unsigned long lastRead = 0;
const int threshold = 230;
int rawValue = 0; 
int brightness = 0;
unsigned long lastStep = 0;

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
      if (millis() - holdStart >= breathInterval) {
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

  if (rawValue >= threshold) {
    glow(100);                  // in micro-seconds
  } else {                    
    breath(100);                // in milli-seconds
  }
}
