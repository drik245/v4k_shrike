#include <esp_now.h>
#include <esp_mac.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_MOSI 23
#define OLED_CLK  18
#define OLED_CS   5 
#define OLED_DC   4
#define OLED_RST  2

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaration for an SPI SSD1306 display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

int scan_count = 0;

// Callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  char msg[len + 1];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';
  
  String msgStr = String(msg);
  Serial.print("Received: ");
  Serial.println(msgStr);
  
  if (msgStr.startsWith("UID:")) {
    String uid = msgStr.substring(4);
    scan_count++;
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(15, 0);
    display.print("RFID Scanned!");
    
    display.setCursor(0, 20);
    display.print("UID:");
    
    display.setCursor(0, 32);
    display.print(uid);
    
    display.setCursor(0, 50);
    display.print("Scan #");
    display.print(scan_count);
    
    display.display();
    
    Serial.print("Displayed UID: ");
    Serial.print(uid);
    Serial.print(" Scan #");
    Serial.println(scan_count);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED (SPI)
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  char macCStr[18];
  snprintf(macCStr, sizeof(macCStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  String macStr = String(macCStr);
  Serial.println("=== ESP-NOW RFID Receiver - ESP32 WROOM ===");
  Serial.print("My MAC address: ");
  Serial.println(macStr);
  Serial.println("Paste this into sender.py as RECEIVER_MAC");
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.print("RFID Receiver");
  display.setCursor(25, 25);
  display.print("Waiting...");
  display.setCursor(0, 50);
  display.print("MAC:");
  display.setCursor(0, 58);
  display.print(macStr);
  display.display();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register for recv CB to get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nothing to do here, ESP-NOW uses interrupts/callbacks
  delay(1000);
}
