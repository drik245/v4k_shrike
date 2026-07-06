# Project 25: RFID Attendance Logger

This MicroPython project turns the Shrike Fi into a Wi-Fi connected smart attendance logger! Whenever a registered RFID card is scanned, it automatically calculates whether the student is arriving (`IN`) or leaving (`OUT`) and logs the event directly to a Google Sheet using a Webhook.

## Hardware Wiring

The MFRC522 communicates over SPI. Just like Project 24, we use Hardware SPI for maximum stability.

### Shrike Fi (ESP32-S3)
| Component | MFRC522 Pin | Shrike Fi Pin |
|-----------|-------------|---------------|
| SCK | SCK | ESP_IO5 |
| MOSI | MOSI | ESP_IO6 |
| MISO | MISO | ESP_IO7 |
| CS / SDA | SDA | ESP_IO4 |
| Reset | RST | ESP_IO3 |
| Green LED | Anode (+) | ESP_IO1 |
| Red LED | Anode (+) | ESP_IO2 |
| Power | 3.3V | 3.3V (Do **not** use 5V for MFRC522) |

## Google Sheets Setup

This project uses Google Apps Script to securely write data to your spreadsheet.

1. Create a new Google Sheet.
2. Click **Extensions > Apps Script**.
3. Paste the following script (which expects `field1` for Name and `field2` for Status):
```javascript
function doPost(e) {
  try {
    var data = JSON.parse(e.postData.contents);
    var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
    
    var timestamp = new Date();
    var name = data.field1 || ""; 
    var status = data.field2 || "";
    
    sheet.appendRow([timestamp, name, status]);
    
    return ContentService.createTextOutput(JSON.stringify({"status": "success"}))
                         .setMimeType(ContentService.MimeType.JSON);
  } catch(error) {
    return ContentService.createTextOutput(JSON.stringify({"status": "error"}))
                         .setMimeType(ContentService.MimeType.JSON);
  }
}
```
4. Click **Deploy > New deployment**.
5. Select **Web app**.
6. Set "Who has access" to **Anyone**.
7. Click Deploy, authorize it, and copy the **Web app URL** provided.

## Board Setup

1. Open `shrike_fi/main.py`.
2. Configure your Wi-Fi credentials (`WIFI_SSID` and `WIFI_PASS`).
3. Paste your Apps Script URL into `WEBHOOK_URL`.
4. Update the `STUDENTS` dictionary with the UIDs of your actual RFID cards and the corresponding student names.
5. Upload `main.py` and `mfrc522.py` to your board (using `mpremote cp`).

## Usage

Once booted, the board will connect to Wi-Fi and print `Ready! Scan a card...` in the console.

- **Scan a known card:** It will log the student as `IN`, flash the green LED, and send the data to Google Sheets.
- **Scan the same card again:** It remembers the previous state and will automatically log the student as `OUT`!
- **Scan an unknown card:** It will flash the red LED and print `Unknown card`.

---
## 💡 MicroPython Tip: Auto-run on Boot
By default, MicroPython boards automatically search for and execute a file named **`main.py`** on boot. If your code is inside a file with a different name (e.g., `app.py`), it will **not** run automatically when you power on the board.

To make it run on boot, you have two options:
1. Rename your script to `main.py`.
2. Create a `main.py` file that simply imports your script:
   ```python
   # Inside main.py
   import app
   ```
