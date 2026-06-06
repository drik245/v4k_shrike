/*
 * DC Motor Web Speed Control – Shrike Fi (ESP32-S3)
 * ==================================================
 * Board target : ESP32-S3 Dev Module (Generic)
 *
 * FPGA generates hardware PWM for L298N motor driver.
 * ESP32-S3 hosts a Wi-Fi AP + web server with a slider UI.
 * MCU sends speed/direction to FPGA over SPI at runtime.
 *
 * Architecture:
 *   Browser → Wi-Fi → ESP32 → SPI → FPGA → PWM → L298N → Motor
 *
 * FPGA I/O (directly wired to L298N):
 *   FPGA_IO8  → L298N ENA  (PWM speed)
 *   FPGA_IO9  → L298N IN1  (direction)
 *   FPGA_IO10 → L298N IN2  (direction)
 *
 * MCU ↔ FPGA bus (Shrike Fi pinout):
 *   ESP_IO12 → FPGA pin 3  (SPI_SCLK)
 *   ESP_IO10 → FPGA pin 4  (SPI_SS)
 *   ESP_IO11 → FPGA pin 5  (SPI_MOSI)
 *   ESP_IO13 → FPGA pin 6  (SPI_MISO)
 *   ESP_IO9  → FPGA EN     (enable)
 *   ESP_IO8  → FPGA PWR    (power control)
 *   ESP_IO21 → onboard LED (status)
 *
 * Ref: https://blog.vicharak.in/7-ways-your-mcu-can-talk-to-fpga-on-shrike/
 */

#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <LittleFS.h>

// ── Wi-Fi Access Point config ──
const char* AP_SSID = "Shrike-Motor";
const char* AP_PASS = "vicharak123";

// ── FPGA bus pins (Shrike Fi — ESP32-S3 GPIOs) ──
// These are the 8-line bus connecting the ESP32-S3 to the SLG47910 FPGA.
// During boot: 4 SPI lines program the bitstream.
// At runtime:  same 4 SPI lines carry motor commands.
#define FPGA_SPI_SCK   12   // ESP_IO12 → FPGA pin 3 (SPI_SCLK)
#define FPGA_SPI_CS    10   // ESP_IO10 → FPGA pin 4 (SPI_SS)
#define FPGA_SPI_MOSI  11   // ESP_IO11 → FPGA pin 5 (SPI_MOSI)
#define FPGA_SPI_MISO  13   // ESP_IO13 → FPGA pin 6 (SPI_MISO)
#define FPGA_EN         9   // ESP_IO9  → FPGA EN  (enable, active HIGH)
#define FPGA_PWR        8   // ESP_IO8  → FPGA PWR (power control)

// ── Status LED ──
#define LED_PIN 21   // ESP_IO21 – onboard LED

// ── Motor state ──
int currentSpeed = 0;  // -255..+255 (negative = reverse)

WebServer server(80);

// ── Web UI (embedded HTML/CSS/JS) ──
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Shrike Motor Control</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;600;700&display=swap');
  * { margin: 0; padding: 0; box-sizing: border-box; }
  body {
    font-family: 'Inter', system-ui, sans-serif;
    background: #0a0a0f;
    color: #e4e4e7;
    min-height: 100vh;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .card {
    background: linear-gradient(145deg, #18181b, #1e1e24);
    border: 1px solid #2a2a32;
    border-radius: 20px;
    padding: 40px 36px;
    width: 380px;
    max-width: 92vw;
    box-shadow: 0 20px 60px rgba(0,0,0,0.5);
  }
  h1 {
    font-size: 1.3rem;
    font-weight: 700;
    text-align: center;
    margin-bottom: 6px;
    background: linear-gradient(135deg, #60a5fa, #a78bfa);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
  }
  .subtitle {
    text-align: center;
    font-size: 0.78rem;
    color: #71717a;
    margin-bottom: 32px;
  }
  .gauge {
    text-align: center;
    margin-bottom: 28px;
  }
  .speed-val {
    font-size: 4rem;
    font-weight: 700;
    line-height: 1;
    font-variant-numeric: tabular-nums;
  }
  .speed-unit {
    font-size: 1rem;
    color: #71717a;
    margin-left: 2px;
  }
  .dir-label {
    font-size: 0.9rem;
    font-weight: 600;
    margin-top: 8px;
    letter-spacing: 0.05em;
    text-transform: uppercase;
    transition: color 0.2s;
  }
  .dir-fwd { color: #34d399; }
  .dir-rev { color: #f87171; }
  .dir-stop { color: #71717a; }

  .slider-wrap { padding: 0 4px; margin-bottom: 24px; }
  input[type=range] {
    -webkit-appearance: none;
    width: 100%;
    height: 8px;
    border-radius: 4px;
    background: #27272a;
    outline: none;
  }
  input[type=range]::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 28px;
    height: 28px;
    border-radius: 50%;
    background: linear-gradient(135deg, #60a5fa, #818cf8);
    cursor: pointer;
    box-shadow: 0 2px 10px rgba(96,165,250,0.4);
    transition: transform 0.15s;
  }
  input[type=range]::-webkit-slider-thumb:hover {
    transform: scale(1.15);
  }
  .labels {
    display: flex;
    justify-content: space-between;
    font-size: 0.72rem;
    color: #52525b;
    margin-top: 6px;
  }
  .btn-row {
    display: flex;
    gap: 10px;
  }
  .btn {
    flex: 1;
    padding: 12px 0;
    border: 1px solid #3f3f46;
    border-radius: 10px;
    background: #27272a;
    color: #e4e4e7;
    font-family: inherit;
    font-size: 0.85rem;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.15s;
  }
  .btn:hover { background: #3f3f46; }
  .btn-stop {
    background: linear-gradient(135deg, #dc2626, #b91c1c);
    border-color: #dc2626;
    color: #fff;
  }
  .btn-stop:hover { background: linear-gradient(135deg, #ef4444, #dc2626); }
  .status {
    text-align: center;
    font-size: 0.72rem;
    color: #52525b;
    margin-top: 20px;
  }
  .dot {
    display: inline-block;
    width: 6px; height: 6px;
    border-radius: 50%;
    background: #22c55e;
    margin-right: 4px;
    vertical-align: middle;
    animation: pulse 2s infinite;
  }
  @keyframes pulse {
    0%, 100% { opacity: 1; }
    50% { opacity: 0.4; }
  }
</style>
</head>
<body>
<div class="card">
  <h1>⚡ Motor Control</h1>
  <p class="subtitle">Shrike Fi · FPGA Hardware PWM</p>

  <div class="gauge">
    <span class="speed-val" id="speedVal">0</span><span class="speed-unit">%</span>
    <div class="dir-label dir-stop" id="dirLabel">STOPPED</div>
  </div>

  <div class="slider-wrap">
    <input type="range" min="-100" max="100" value="0" id="slider" />
    <div class="labels">
      <span>◀ REV 100%</span>
      <span>0</span>
      <span>FWD 100% ▶</span>
    </div>
  </div>

  <div class="btn-row">
    <button class="btn" onclick="setSpeed(-100)">Full REV</button>
    <button class="btn btn-stop" onclick="setSpeed(0)">STOP</button>
    <button class="btn" onclick="setSpeed(100)">Full FWD</button>
  </div>

  <div class="status"><span class="dot"></span>Connected via Wi-Fi</div>
</div>

<script>
  const slider = document.getElementById('slider');
  const speedVal = document.getElementById('speedVal');
  const dirLabel = document.getElementById('dirLabel');
  let timeout = null;

  function updateUI(pct) {
    speedVal.textContent = Math.abs(pct);
    dirLabel.className = 'dir-label ' + (pct > 0 ? 'dir-fwd' : pct < 0 ? 'dir-rev' : 'dir-stop');
    dirLabel.textContent = pct > 0 ? 'FORWARD' : pct < 0 ? 'REVERSE' : 'STOPPED';
  }

  function sendSpeed(pct) {
    const speed = Math.round(pct * 2.55);
    fetch('/set?speed=' + speed).catch(() => {});
  }

  function setSpeed(pct) {
    slider.value = pct;
    updateUI(pct);
    sendSpeed(pct);
  }

  slider.addEventListener('input', function() {
    const pct = parseInt(this.value);
    updateUI(pct);
    clearTimeout(timeout);
    timeout = setTimeout(() => sendSpeed(pct), 30);
  });

  slider.addEventListener('change', function() {
    sendSpeed(parseInt(this.value));
  });
</script>
</body>
</html>
)rawliteral";

// ── FPGA Programming ────────────────────────────────────────────────────
// Programs the SLG47910 FPGA with the bitstream stored in LittleFS.
// Follows the power-cycle → load → enable sequence described in:
//   https://blog.vicharak.in/7-ways-your-mcu-can-talk-to-fpga-on-shrike/
//
// The bitstream file lives at /bitstream.bin on the ESP32's flash filesystem.
// Upload it using the Arduino IDE LittleFS upload tool or platformio.
//
// Phase 1: Power-cycle FPGA
// Phase 2: Read bitstream from LittleFS, clock out via SPI
// Phase 3: Pulse EN to activate the design
// After this, the 4 SPI lines become our runtime data bus.

#define BITSTREAM_PATH "/bitstream.bin"

void programFPGA() {
  Serial.println("[FPGA] Programming FPGA bitstream...");

  // Mount LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("[FPGA] ERROR: LittleFS mount failed!");
    Serial.println("[FPGA] Upload the data/ folder using the LittleFS upload tool.");
    return;
  }

  // Open bitstream file
  File bsFile = LittleFS.open(BITSTREAM_PATH, "r");
  if (!bsFile) {
    Serial.printf("[FPGA] ERROR: %s not found!\n", BITSTREAM_PATH);
    Serial.println("[FPGA] Place your compiled bitstream in data/bitstream.bin");
    Serial.println("[FPGA] and re-upload using the LittleFS upload tool.");
    LittleFS.end();
    return;
  }

  size_t fileSize = bsFile.size();
  Serial.printf("[FPGA] Bitstream: %s (%d bytes)\n", BITSTREAM_PATH, fileSize);

  // Step 1: Power-cycle the FPGA
  pinMode(FPGA_PWR, OUTPUT);
  pinMode(FPGA_EN, OUTPUT);
  digitalWrite(FPGA_EN, LOW);
  digitalWrite(FPGA_PWR, LOW);
  delay(10);
  digitalWrite(FPGA_PWR, HIGH);
  delay(10);
  Serial.println("[FPGA] Power-cycled");

  // Step 2: Clock out bitstream via SPI (MCU is master, FPGA is slave)
  SPI.begin(FPGA_SPI_SCK, FPGA_SPI_MISO, FPGA_SPI_MOSI, FPGA_SPI_CS);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  digitalWrite(FPGA_SPI_CS, LOW);

  uint8_t buf[256];
  size_t totalSent = 0;
  while (bsFile.available()) {
    size_t bytesRead = bsFile.read(buf, sizeof(buf));
    for (size_t i = 0; i < bytesRead; i++) {
      SPI.transfer(buf[i]);
    }
    totalSent += bytesRead;
  }

  digitalWrite(FPGA_SPI_CS, HIGH);
  SPI.endTransaction();
  delay(5);

  bsFile.close();
  LittleFS.end();

  // Step 3: Pulse EN to activate the loaded design
  digitalWrite(FPGA_EN, HIGH);
  delay(10);
  Serial.printf("[FPGA] Done — %d bytes sent, design active\n", totalSent);
}

// ── Send Motor Command to FPGA ──────────────────────────────────────────
// Sends a 2-byte SPI frame:
//   Byte 0: direction  (0x00=STOP, 0x01=FWD, 0x02=REV)
//   Byte 1: duty cycle (0–255)

void sendMotorCommand(int speed) {
  uint8_t dir;
  uint8_t duty;

  if (speed > 0) {
    dir  = 0x01;  // forward
    duty = constrain(speed, 0, 255);
  } else if (speed < 0) {
    dir  = 0x02;  // reverse
    duty = constrain(-speed, 0, 255);
  } else {
    dir  = 0x00;  // stop
    duty = 0;
  }

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(FPGA_SPI_CS, LOW);
  SPI.transfer(dir);
  SPI.transfer(duty);
  digitalWrite(FPGA_SPI_CS, HIGH);
  SPI.endTransaction();
}

// ── Web Server Handlers ─────────────────────────────────────────────────

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleSet() {
  if (server.hasArg("speed")) {
    currentSpeed = server.arg("speed").toInt();
    currentSpeed = constrain(currentSpeed, -255, 255);
    sendMotorCommand(currentSpeed);

    Serial.printf("[WEB] speed=%d  dir=%s  duty=%d\n",
                  currentSpeed,
                  currentSpeed > 0 ? "FWD" : currentSpeed < 0 ? "REV" : "STOP",
                  abs(currentSpeed));
  }
  server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

// ── Setup ───────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  Serial.println("\n===== DC Motor Web Speed Control (Shrike Fi) =====");

  // Status LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // FPGA control + SPI CS pins
  pinMode(FPGA_SPI_CS, OUTPUT);
  digitalWrite(FPGA_SPI_CS, HIGH);

  // Program FPGA bitstream (power-cycle → SPI load → enable)
  programFPGA();

  // Re-init SPI for runtime communication (higher speed)
  // After shrike.flash() releases the bus, the 4 SPI lines become our data channel
  SPI.begin(FPGA_SPI_SCK, FPGA_SPI_MISO, FPGA_SPI_MOSI, FPGA_SPI_CS);
  Serial.println("[SPI]  Runtime SPI ready (4 MHz)");

  // Start Wi-Fi AP
  Serial.printf("[WIFI] Starting AP: %s\n", AP_SSID);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress ip = WiFi.softAPIP();
  Serial.printf("[WIFI] AP ready — connect and open http://%s\n", ip.toString().c_str());

  // Start web server
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("[WEB]  Server started on port 80");

  // LED on = ready
  digitalWrite(LED_PIN, HIGH);
  Serial.println("[RDY]  System ready\n");
}

// ── Loop ────────────────────────────────────────────────────────────────

void loop() {
  server.handleClient();

  // Blink LED while motor is active
  static unsigned long lastBlink = 0;
  if (currentSpeed != 0) {
    if (millis() - lastBlink > 200) {
      lastBlink = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  } else {
    digitalWrite(LED_PIN, HIGH);  // solid = idle
  }
}
