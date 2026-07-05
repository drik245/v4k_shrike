# Project 23: OTA Update via GitHub

This MicroPython project enables **Over-The-Air (OTA) updates** directly from a GitHub repository for the Shrike Fi (ESP32-S3). 

Instead of plugging your board into a computer every time you want to change its behavior, the board can check a specific folder on your GitHub repository for a new version of `main.py` and automatically download and install it!

## How It Works

The board runs a script (`ota.py`) which performs the following steps:
1. Connects to your Wi-Fi network.
2. Reads the current version from a local file (`local_version.txt`).
3. Fetches the latest `version.txt` from your specified GitHub repository.
4. Compares the two versions. If the GitHub version is different (e.g. `1.0.1` vs `1.0.0`), it downloads the new `main.py` from GitHub.
5. Overwrites the local `main.py`, updates `local_version.txt`, and physically resets the board to run the new code!

## Setup Instructions

### 1. Configure GitHub Details
Open `shrike_fi/ota.py` and configure the constants at the top of the file to point to your repository and Wi-Fi:

```python
WIFI_SSID     = "YOUR_WIFI_SSID"
WIFI_PASS     = "YOUR_WIFI_PASSWORD"
GITHUB_USER   = "your-username"
GITHUB_REPO   = "your-repo-name"
GITHUB_BRANCH = "main"
REMOTE_PATH   = "path/to/project/folder" # e.g. "23_ota_github_update/shrike_fi"
```

### 2. Prepare the GitHub Repository
In your GitHub repository, ensure you have the following two files in the path specified by `REMOTE_PATH`:
- `main.py`: The actual MicroPython script you want the board to run.
- `version.txt`: A simple text file containing a version string (e.g., `1.0.0`).

### 3. Flash the Board
You need to manually flash the board **one time** to get the OTA system running.
Upload `ota.py`, a base `main.py`, and create a `local_version.txt` with `1.0.0` on your Shrike Fi.

In your `main.py`, you should import and call the OTA check at the very beginning, so it checks for updates on boot:
```python
import ota

# Check for updates first thing!
ota.check_for_update()

# ... rest of your main code here ...
```

## Releasing an Update

When you want to update the code on your board wirelessly:
1. Edit `main.py` in your GitHub repository and commit the changes.
2. Edit `version.txt` (e.g. change `1.0.0` to `1.0.1`) and commit the change.
3. The next time the Shrike Fi reboots (or whenever you call `ota.check_for_update()`), it will detect the new version, download the new `main.py`, and restart automatically with the new code!
