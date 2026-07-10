/**
 * Project 29: Internet Radio (Shrike Fi / ESP32-S3)
 * Features an OLED display and two buttons to cycle through radio stations.
 * Uses PDM via I2S and an RC low-pass filter for audio output.
 * 
 * Required Libraries:
 * - arduino-audio-tools, arduino-libhelix
 * - Adafruit GFX Library
 * - Adafruit SSD1306
 */

#include "AudioTools.h"
#include "AudioTools/Communication/AudioHttp.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// CONFIGURATION
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Hardware Pins
const int AUDIO_OUT_PIN = 18;
const int BTN_NEXT_PIN = 14;
const int BTN_PREV_PIN = 7;

// OLED uses Software SPI on safe pins
#define OLED_MOSI   1
#define OLED_CLK    2
#define OLED_DC     3
#define OLED_CS     4
#define OLED_RESET  5

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Radio Stations
struct Station {
  const char* name;
  const char* url;
};

Station stations[] = {
  {"Groove Salad",  "http://ice1.somafm.com/groovesalad-128-mp3"},
  {"DEF CON Radio", "http://ice1.somafm.com/defcon-128-mp3"},
  {"Beat Blender",  "http://ice1.somafm.com/beatblender-128-mp3"},
  {"Swiss Jazz",    "http://stream.srg-ssr.ch/m/rsj/mp3_128"},
  {"Drone Zone",    "http://ice1.somafm.com/dronezone-128-mp3"}
};
const int TOTAL_STATIONS = 5;
int current_station = 0;

// Audio Objects
URLStream url; 
I2SStream out; 
EncodedAudioStream dec(&out, new MP3DecoderHelix());  
StreamCopy copier(dec, url); 

// Button State
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 300;


// ── Display helpers ──

// ascending signal bars for wifi strength
void drawSignalBars(int x, int y) {
    int bars = 0;
    if (WiFi.status() == WL_CONNECTED) {
        int rssi = WiFi.RSSI();
        if      (rssi > -50) bars = 4;
        else if (rssi > -60) bars = 3;
        else if (rssi > -70) bars = 2;
        else                 bars = 1;
    }
    for (int i = 0; i < 4; i++) {
        int h = 2 + i * 2;
        int by = y + 8 - h;
        if (i < bars)
            display.fillRect(x + i * 4, by, 3, h, SSD1306_WHITE);
        else
            display.drawRect(x + i * 4, by, 3, h, SSD1306_WHITE);
    }
}

// top bar: status text, station counter, wifi icon
void drawStatusBar(const char* label) {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(label);

    char buf[8];
    snprintf(buf, sizeof(buf), "%d/%d", current_station + 1, TOTAL_STATIONS);
    int16_t x1, y1; uint16_t tw, th;
    display.getTextBounds(buf, 0, 0, &x1, &y1, &tw, &th);
    display.setCursor(SCREEN_WIDTH - 18 - tw, 0);
    display.print(buf);

    drawSignalBars(SCREEN_WIDTH - 16, 0);
}

void drawFooter() {
    display.setTextSize(1);
    display.setCursor(0, 56);
    display.print("< Prev");
    display.setCursor(SCREEN_WIDTH - 36, 56);
    display.print("Next >");
}

void updateDisplay(const char* label) {
    display.clearDisplay();
    drawStatusBar(label);
    display.drawLine(0, 10, SCREEN_WIDTH - 1, 10, SSD1306_WHITE);

    // station name, size 1 so long names don't overflow
    display.setTextSize(1);
    display.setCursor(0, 26);
    display.print(stations[current_station].name);

    drawFooter();
    display.display();
}


// ── Station switching ──

void changeStation(int direction) {
    current_station += direction;
    if (current_station >= TOTAL_STATIONS) current_station = 0;
    if (current_station < 0) current_station = TOTAL_STATIONS - 1;

    updateDisplay("Switching...");

    // only reset url and decoder, don't touch copier
    url.end();
    dec.end();

    delay(300);

    dec.begin();
    Serial.print("Connecting to: ");
    Serial.println(stations[current_station].url);
    url.begin(stations[current_station].url, "audio/mp3");
    Serial.println("Stream started.");

    updateDisplay("Playing:");
}


// ── Arduino entry points ──

void setup() {
    Serial.begin(115200);
    AudioLogger::instance().begin(Serial, AudioLogger::Warning);

    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_PREV_PIN, INPUT_PULLUP);

    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
        Serial.println("SSD1306 allocation failed");
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 28);
    display.println("Connecting Wi-Fi...");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Wi-Fi Connected!");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(1000);

    // PDM audio output
    auto config = out.defaultConfig(TX_MODE);
    config.signal_type = PDM;
    config.pin_data = AUDIO_OUT_PIN;
    config.pin_bck = 17;
    out.begin(config);

    dec.begin();
    updateDisplay("Buffering...");
    Serial.print("Connecting to: ");
    Serial.println(stations[current_station].url);
    url.begin(stations[current_station].url, "audio/mp3");
    Serial.println("Stream started.");
    updateDisplay("Playing:");
}

void loop() {
    if (!copier.copy()) {
        delay(1);
    }

    if (millis() - lastDebounceTime > debounceDelay) {
        if (digitalRead(BTN_NEXT_PIN) == LOW) {
            lastDebounceTime = millis();
            changeStation(1);
        }
        else if (digitalRead(BTN_PREV_PIN) == LOW) {
            lastDebounceTime = millis();
            changeStation(-1);
        }
    }
}
