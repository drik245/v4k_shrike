# Project 09: Telegram Alert System

This project turns your Shrike Fi (ESP32-S3) into a smart motion detection alert system. It uses an IR sensor to detect motion and sends an instant alert directly to your phone via Telegram. 

It also supports two-way communication: you can send `/status` to the bot to check its uptime and motion count!

## Hardware Wiring

| Component | Shrike Fi Pin | Description |
|---|---|---|
| IR Sensor OUT | ESP_IO4 (GPIO 4) | Motion detection pin (active low) |
| LED Anode (+) | ESP_IO21 (GPIO 21) | Status indicator LED |
| Power / Ground | 3V3 / GND | Supply power to the sensor |

## Libraries Required
Install these from the Arduino IDE Library Manager:
* **UniversalTelegramBot** by Brian Lough
* **ArduinoJson** by Benoit Blanchon

## Setup Guide: Creating your Telegram Bot

To send messages, you need to create a bot on Telegram and get your unique `BOTtoken` and `CHAT_ID`.

### Step 1: Create the Bot and get the BOTtoken
1. Open the Telegram app on your phone or PC.
2. Search for **@BotFather** and open the chat.
3. Send the command `/newbot`.
4. Follow the prompts: give your bot a name (e.g., "My Shrike Alert") and a username (must end in "bot", e.g., "ShrikeAlertBot").
5. BotFather will reply with a message containing your **bot token**. It will look something like this: `6969696969:DJhjjfkjdkllLKJKjkjidjiduamnvpowryslsn`.
6. Copy this token and paste it into the Arduino sketch where it says `#define BOTtoken`.

### Step 2: Start the Bot
Bots cannot initiate conversations with users. You must message it first!
1. In Telegram, search for your bot's username (e.g., @ShrikeAlertBot).
2. Open the chat and tap **Start** (or send the command `/start`). 

### Step 3: Get your CHAT_ID
Now that you have started a chat with your bot, you need your personal Chat ID so the Shrike Fi knows who to send the alerts to.
1. In Telegram, search for **@userinfobot** (or **@IDBot**).
2. Open the chat and tap **Start** or send `/start`.
3. The bot will reply with your ID (a string of numbers, e.g., `6969696969`).
4. Copy this ID and paste it into the Arduino sketch where it says `#define CHAT_ID`.

## Usage
1. Open `telegram_alert.ino`.
2. Update the `ssid`, `password`, `BOTtoken`, and `CHAT_ID` with your details.
3. Upload the code to your Shrike Fi.
4. When motion is detected, the LED will flash, and you'll receive a Telegram message!
5. Send `/status` to your bot in Telegram to see the current uptime and total motion events.

> **Note on SSL:** This code uses `client.setInsecure()` to bypass SSL certificate validation for simplicity, which prevents silent connection failures when the Telegram root certificate updates.
