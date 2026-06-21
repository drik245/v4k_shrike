/*
  ClimaPixel Weather Display - Shrike Fi (ESP32-S3)

  Reads temperature and humidity from a DHT22 sensor and displays
  them on an SSD1306 OLED with custom bitmap icons (thermometer
  and water droplet). No WiFi needed, all local.

  Wiring:
    DHT22 data - ESP_IO4 (GPIO 4)
    OLED SPI: MOSI-35, CLK-36, DC-37, RST-38, CS-34

  Needs: DHTesp (beegee-tokyo), Adafruit SSD1306, Adafruit GFX
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64

// oled spi pins
#define OLED_MOSI 35
#define OLED_CLK  36
#define OLED_DC   37
#define OLED_RST  38
#define OLED_CS   34

// sensor pin
#define DHTPIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

DHTesp dht;

// thermometer icon (16x16)
static const unsigned char PROGMEM iconTemp[] = {
  0x01, 0x80,
  0x02, 0x40,
  0x02, 0x40,
  0x02, 0x40,
  0x03, 0xC0,
  0x02, 0x40,
  0x03, 0xC0,
  0x02, 0x40,
  0x03, 0xC0,
  0x02, 0x40,
  0x07, 0xE0,
  0x0F, 0xF0,
  0x0F, 0xF0,
  0x0F, 0xF0,
  0x07, 0xE0,
  0x03, 0xC0,
};

// water droplet icon (16x16)
static const unsigned char PROGMEM iconHum[] = {
  0x01, 0x80, 
  0x01, 0x80, 
  0x03, 0xC0, 
  0x03, 0xC0, 
  0x07, 0xE0, 
  0x07, 0xE0, 
  0x0F, 0xF0, 
  0x0F, 0xF0, 
  0x1F, 0xF8, 
  0x1F, 0xF8, 
  0x1F, 0xF8, 
  0x1F, 0xF8, 
  0x0F, 0xF0, 
  0x0F, 0xF0, 
  0x07, 0xE0, 
  0x01, 0x80, 
};

// cached readings for display smoothing
float lastTemp = NAN;
float lastHum  = NAN;

void setup() {
  Serial.begin(115200);

  dht.setup(DHTPIN, DHTesp::DHT22);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // splash screen
  display.setTextSize(2);
  display.setCursor(6, 8);
  display.print("ClimaPixel");
  display.setTextSize(1);
  display.setCursor(24, 36);
  display.print("Weather Station");
  display.setCursor(36, 50);
  display.print("Loading...");
  display.display();
  delay(2000);
}

void loop() {
  TempAndHumidity data = dht.getTempAndHumidity();

  if (dht.getStatus() == DHTesp::ERROR_NONE) {
    lastTemp = data.temperature;
    lastHum  = data.humidity;
  }

  display.clearDisplay();

  // title
  display.setTextSize(1);
  int titleW = 13 * 6; // "Mini Weather" is 12 chars + margin
  display.setCursor((128 - 12 * 6) / 2, 2);
  display.print("Mini Weather");

  // separator line
  display.drawLine(0, 12, 127, 12, SSD1306_WHITE);

  if (isnan(lastTemp)) {
    // sensor error
    display.setTextSize(1);
    display.setCursor(10, 30);
    display.print("Sensor Error");
    display.display();
    delay(2000);
    return;
  }

  // temperature row
  display.drawBitmap(8, 18, iconTemp, 16, 16, SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(30, 20);
  display.print(lastTemp, 1);
  display.setTextSize(1);
  display.print(" C");

  // humidity row
  display.drawBitmap(8, 40, iconHum, 16, 16, SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(30, 42);
  display.print(lastHum, 0);
  display.setTextSize(1);
  display.print(" %");

  display.display();

  Serial.print("temp: ");
  Serial.print(lastTemp, 1);
  Serial.print("C  hum: ");
  Serial.print(lastHum, 0);
  Serial.println("%");

  delay(2000);
}
