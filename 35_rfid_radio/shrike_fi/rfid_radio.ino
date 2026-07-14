#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Audio.h> // ESP32-audioI2S library

// ---------------------------------------------------------
// PIN DEFINITIONS (Shrike Fi - ESP32-S3 Only)
// ---------------------------------------------------------
#define RST_PIN  5 // ESP_IO5
#define SS_PIN   6 // ESP_IO6

// Initialize RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Initialize Audio
Audio audio;

// Wi-Fi
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// ---------------------------------------------------------
// RFID UIDs mapped to Radio Station URLs
// ---------------------------------------------------------
// NOTE: Scan your tags once and check the Serial Monitor to get their exact UIDs, then update these strings!
String tag1 = "04 15 E7 22"; 
String url1 = "http://ice1.somafm.com/groovesalad-128-mp3";

String tag2 = "A3 4B 9C 11"; 
String url2 = "http://ice1.somafm.com/defcon-128-mp3";

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // PDM Audio setup (Similar to Project 29)
  // Pin 4 is used for I2S DOUT
  audio.setPinout(I2S_BCLK, I2S_LRC, 4); 
  audio.setVolume(12);
  
  Serial.println("RFID Radio Ready! Tap a card to play a station.");
}

void loop() {
  // MUST call audio.loop() constantly to keep the buffer full
  audio.loop();

  // Check for new RFID cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    uid.trim();
    uid.toUpperCase();
    
    Serial.print("\nCard Scanned: ");
    Serial.println(uid);

    // Stop current stream before starting new one
    audio.stopSong();
    
    if (uid == tag1) {
      Serial.println("Tag 1 recognized! Playing Groove Salad...");
      audio.connecttohost(url1.c_str());
    } else if (uid == tag2) {
      Serial.println("Tag 2 recognized! Playing DEF CON Radio...");
      audio.connecttohost(url2.c_str());
    } else {
      Serial.println("Unknown card. (Add this UID to your code to assign a station!)");
    }
    
    // Halt PICC to prevent scanning the same card repeatedly while held
    mfrc522.PICC_HaltA();
  }
}
