#include <LiquidCrystal.h>
#include <DHTesp.h>

// pins
#define RS 2
#define EN 1
#define D4 3
#define D5 4
#define D6 5
#define D7 6
#define DHT_PIN 7

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
DHTesp dht;

//setup
void setup() {
  lcd.begin(16, 2);
  lcd.print("Weather Station");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  dht.setup(DHT_PIN, DHTesp::DHT22);
}

void loop() {
  delay(2000); //wait 2 sec
  TempAndHumidity data = dht.getTempAndHumidity();
  if (dht.getStatus() == DHTesp::ERROR_NONE) {

    // Display temperature
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(data.temperature, 1);
    lcd.print(" C    ");

    // Display humidity
    lcd.setCursor(0, 1);
    lcd.print("Hum:  ");
    lcd.print(data.humidity, 1);
    lcd.print(" %    ");
  } else {
    // Show error message
    lcd.setCursor(0, 0);
    lcd.print("Error!    ");
    lcd.setCursor(0, 1);
    lcd.print("Reading DHT...  ");
  }
}


