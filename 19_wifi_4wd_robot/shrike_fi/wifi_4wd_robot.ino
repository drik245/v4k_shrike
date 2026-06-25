/*
  Wi-Fi Controlled 4WD Robot Car - Shrike Fi (ESP32-S3)

  Control a 4WD robot car over Wi-Fi! The ESP32 creates an Access Point (AP)
  and hosts a web interface to drive the car from your phone.
  
  Features:
  - Full directional control (Forward, Backward, Left, Right, Stop)
  - Speed control slider (PWM)
  - Ultrasonic sensor mounted on a Servo for obstacle detection
  - Auto-stops and sounds a reverse buzzer if an obstacle is too close.

  Wiring (16 GPIOs total!):
  
  Driver 1 (Front Motors):
    IN1 - ESP_IO1
    IN2 - ESP_IO2
    IN3 - ESP_IO3
    IN4 - ESP_IO4
    ENA - ESP_IO5 (PWM)
    ENB - ESP_IO6 (PWM)

  Driver 2 (Rear Motors):
    IN1 - ESP_IO7
    IN2 - ESP_IO8
    IN3 - ESP_IO9
    IN4 - ESP_IO10
    ENA - ESP_IO11 (PWM)
    ENB - ESP_IO12 (PWM)
    
  Sensors & Actuators:
    Trig (HC-SR04) - ESP_IO13
    Echo (HC-SR04) - ESP_IO14
    Servo Motor    - ESP_IO15
    Buzzer         - ESP_IO16
    
  Needs: ESP32Servo library.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// wifi ap credentials
const char* ssid     = "Shrike-4WD";
const char* password = "password123";

WebServer server(80);

// pins
// front driver
const int F_IN1 = 48;
const int F_IN2 = 47;
const int F_IN3 = 46;
const int F_IN4 = 45;
const int F_ENA = 17;
const int F_ENB = 18;

// rear driver
const int R_IN1 = 38;
const int R_IN2 = 37;
const int R_IN3 = 36;
const int R_IN4 = 35;
const int R_ENA = 15;
const int R_ENB = 16;

// sensors / actuators
const int TRIG_PIN = 1;
const int ECHO_PIN = 2;
const int SERVO_PIN = 3;
const int BUZZER_PIN = 14;

Servo scannerServo;

// pwm properties
const int freq = 5000;
const int resolution = 8;
int currentSpeed = 240;

// state
String currentAction = "stop";
unsigned long lastPingTime = 0;
float distance_cm = 100.0;
bool obstacleDetected = false;

// servo sweep state
int servoPos = 90;
int servoDir = 1;
unsigned long lastServoTime = 0;

// reverse buzzer state
unsigned long lastBuzzerToggle = 0;
bool buzzerState = false;

// html ui
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <meta charset="UTF-8">
  <title>Shrike 4WD Control</title>
  <style>
    * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
    body { font-family: Arial; text-align: center; background-color: #2c3e50; color: white; margin: 0; padding: 20px; touch-action: manipulation; }
    h1 { margin-bottom: 5px; }
    #status { font-size: 1.2em; color: #f1c40f; margin-bottom: 20px; }
    .grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 15px; max-width: 300px; margin: 0 auto; }
    .btn {
      padding: 25px 0; font-size: 24px; font-weight: bold;
      border: none; border-radius: 10px; background-color: #3498db;
      color: white; cursor: pointer; user-select: none;
      box-shadow: 0 4px #2980b9;
    }
    .btn:active { background-color: #2980b9; box-shadow: 0 2px #2980b9; transform: translateY(2px); }
    .empty { background: none; box-shadow: none; cursor: default; }
    .stop-btn { background-color: #e74c3c; box-shadow: 0 4px #c0392b; }
    .stop-btn:active { background-color: #c0392b; box-shadow: 0 2px #c0392b; }
    .slider-container { margin-top: 30px; }
    input[type=range] { width: 80%; max-width: 300px; }
  </style>
</head>
<body>
  <h1>Shrike 4WD</h1>
  <div id="status">Status: Ready</div>

  <div class="grid">
    <div class="empty"></div>
    <button class="btn" data-down="forward" data-up="stop">FWD</button>
    <div class="empty"></div>
    <button class="btn" data-down="left" data-up="stop">LEFT</button>
    <button class="btn stop-btn" data-down="stop">STOP</button>
    <button class="btn" data-down="right" data-up="stop">RIGHT</button>
    <div class="empty"></div>
    <button class="btn" data-down="backward" data-up="stop">REV</button>
    <div class="empty"></div>
  </div>

  <div class="slider-container">
    <h3>Speed: <span id="speedVal">240</span></h3>
    <input type="range" min="100" max="240" value="240" id="speedSlider">
  </div>

  <script>
    // abort any in-flight request before sending a new one
    // this prevents the esp from choking on stacked requests
    var ctrl = null;

    function send(cmd) {
      if (ctrl) ctrl.abort();
      ctrl = new AbortController();
      fetch('/' + cmd, { signal: ctrl.signal }).catch(function(){});
      document.getElementById('status').innerText = 'Status: ' + cmd.toUpperCase();
    }

    // attach events to all buttons via data attributes
    document.querySelectorAll('.btn').forEach(function(btn) {
      var downCmd = btn.getAttribute('data-down');
      var upCmd = btn.getAttribute('data-up');

      // touch events (mobile) - use these and block mouse events
      btn.addEventListener('touchstart', function(e) {
        e.preventDefault();
        send(downCmd);
      });
      if (upCmd) {
        btn.addEventListener('touchend', function(e) {
          e.preventDefault();
          send(upCmd);
        });
      }

      // mouse events (desktop fallback only)
      btn.addEventListener('mousedown', function() { send(downCmd); });
      if (upCmd) {
        btn.addEventListener('mouseup', function() { send(upCmd); });
      }
    });

    // speed slider
    document.getElementById('speedSlider').addEventListener('input', function() {
      var val = this.value;
      document.getElementById('speedVal').innerText = val;
      if (ctrl) ctrl.abort();
      ctrl = new AbortController();
      fetch('/speed?v=' + val, { signal: ctrl.signal }).catch(function(){});
    });
  </script>
</body>
</html>
)rawliteral";

// motor logic

void setSpeed(int speed) {
  analogWrite(F_ENA, speed);
  analogWrite(F_ENB, speed);
  analogWrite(R_ENA, speed);
  analogWrite(R_ENB, speed);
}

void motorsStop() {
  currentAction = "stop";
  setSpeed(0);
  digitalWrite(F_IN1, LOW); digitalWrite(F_IN2, LOW);
  digitalWrite(F_IN3, LOW); digitalWrite(F_IN4, LOW);
  digitalWrite(R_IN1, LOW); digitalWrite(R_IN2, LOW);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, LOW);
}

void motorsForward() {
  if(obstacleDetected) return; // blocked
  currentAction = "forward";
  // set direction first, then enable speed
  digitalWrite(F_IN1, HIGH); digitalWrite(F_IN2, LOW);
  digitalWrite(F_IN3, HIGH); digitalWrite(F_IN4, LOW);
  digitalWrite(R_IN1, HIGH); digitalWrite(R_IN2, LOW);
  digitalWrite(R_IN3, HIGH); digitalWrite(R_IN4, LOW);
  setSpeed(currentSpeed);
}

void motorsBackward() {
  currentAction = "backward";
  digitalWrite(F_IN1, LOW); digitalWrite(F_IN2, HIGH);
  digitalWrite(F_IN3, LOW); digitalWrite(F_IN4, HIGH);
  digitalWrite(R_IN1, LOW); digitalWrite(R_IN2, HIGH);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, HIGH);
  setSpeed(currentSpeed);
}

void motorsLeft() {
  if(obstacleDetected) return; // blocked
  currentAction = "left";
  // left side backward, right side forward (tank turn)
  digitalWrite(F_IN1, HIGH); digitalWrite(F_IN2, LOW);  // front right fwd
  digitalWrite(F_IN3, LOW);  digitalWrite(F_IN4, HIGH); // front left rev
  digitalWrite(R_IN1, HIGH); digitalWrite(R_IN2, LOW);  // rear right fwd
  digitalWrite(R_IN3, LOW);  digitalWrite(R_IN4, HIGH); // rear left rev
  setSpeed(currentSpeed);
}

void motorsRight() {
  if(obstacleDetected) return; // blocked
  currentAction = "right";
  // right side backward, left side forward (tank turn)
  digitalWrite(F_IN1, LOW);  digitalWrite(F_IN2, HIGH); // front right rev
  digitalWrite(F_IN3, HIGH); digitalWrite(F_IN4, LOW);  // front left fwd
  digitalWrite(R_IN1, LOW);  digitalWrite(R_IN2, HIGH); // rear right rev
  digitalWrite(R_IN3, HIGH); digitalWrite(R_IN4, LOW);  // rear left fwd
  setSpeed(currentSpeed);
}

// web handlers

void handleRoot() {
  server.send(200, "text/html", index_html);
}
void handleForward() { motorsForward(); server.send(200, "text/plain", "OK"); }
void handleBackward() { motorsBackward(); server.send(200, "text/plain", "OK"); }
void handleLeft() { motorsLeft(); server.send(200, "text/plain", "OK"); }
void handleRight() { motorsRight(); server.send(200, "text/plain", "OK"); }
void handleStop() { motorsStop(); server.send(200, "text/plain", "OK"); }
void handleSpeed() {
  if (server.hasArg("v")) {
    currentSpeed = server.arg("v").toInt();
    if(currentAction != "stop") setSpeed(currentSpeed);
  }
  server.send(200, "text/plain", "OK");
}

// ultrasonic logic

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if(duration == 0) return 999.0;
  return (duration * 0.0343) / 2.0;
}

void setup() {
  Serial.begin(115200);

  // setup motor pins
  int motorPins[] = {F_IN1, F_IN2, F_IN3, F_IN4, F_ENA, F_ENB, 
                     R_IN1, R_IN2, R_IN3, R_IN4, R_ENA, R_ENB};
  for(int i=0; i<12; i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }

  // sensors & actuators
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // servo
  ESP32PWM::allocateTimer(0);
  scannerServo.setPeriodHertz(50);
  scannerServo.attach(SERVO_PIN, 500, 2400);
  scannerServo.write(90); // center

  // start wifi ap
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // setup web server routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  server.on("/speed", handleSpeed);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // servo sweeping (scan back and forth)
  if (millis() - lastServoTime > 15) {
    lastServoTime = millis();
    servoPos += servoDir;
    if (servoPos <= 30 || servoPos >= 150) servoDir = -servoDir;
    scannerServo.write(servoPos);
  }

  // ultrasonic ping every 50ms
  if (millis() - lastPingTime > 50) {
    lastPingTime = millis();
    distance_cm = readDistance();
    
    // check obstacle (e.g. closer than 5 cm)
    if (distance_cm < 5.0) {
      if (!obstacleDetected) {
        obstacleDetected = true;
        Serial.println("Obstacle! Stopping.");
        // stop if moving forward, left, or right (not backward)
        if(currentAction != "stop" && currentAction != "backward") motorsStop();
      }
    } else {
      obstacleDetected = false;
    }
  }

  // reverse buzzer logic
  if (currentAction == "backward") {
    // beep every 300ms
    if (millis() - lastBuzzerToggle > 300) {
      lastBuzzerToggle = millis();
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    }
  } else if (obstacleDetected) {
    // rapid beep if blocked
    if (millis() - lastBuzzerToggle > 100) {
      lastBuzzerToggle = millis();
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    }
  } else {
    // quiet
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = false;
  }
}
