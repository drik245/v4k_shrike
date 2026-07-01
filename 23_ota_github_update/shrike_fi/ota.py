# Project 23: OTA Update via GitHub - Shrike Fi (ESP32-S3)
# ota.py - Handles Wi-Fi connection and GitHub update logic

import network
import urequests
import machine
import time
import os

# ===== CONFIGURE THESE =====
WIFI_SSID     = "YOUR_WIFI_SSID"
WIFI_PASS     = "YOUR_WIFI_PASSWORD"
GITHUB_USER   = "YOUR_GITHUB_USERNAME"
GITHUB_REPO   = "YOUR_REPO_NAME"
GITHUB_BRANCH = "main"
# ============================

BASE_URL = "https://raw.githubusercontent.com/{}/{}/{}/".format(
    GITHUB_USER, GITHUB_REPO, GITHUB_BRANCH
)

VERSION_FILE_REMOTE = BASE_URL + "version.txt"
MAIN_FILE_REMOTE    = BASE_URL + "main.py"
LOCAL_VERSION_FILE  = "local_version.txt"


def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)

    if wlan.isconnected():
        print("Wi-Fi already connected:", wlan.ifconfig()[0])
        return True

    print("Connecting to Wi-Fi:", WIFI_SSID)
    wlan.connect(WIFI_SSID, WIFI_PASS)

    timeout = 15
    while not wlan.isconnected() and timeout > 0:
        time.sleep(1)
        timeout -= 1
        print(".", end="")

    if wlan.isconnected():
        print("\nConnected! IP:", wlan.ifconfig()[0])
        return True
    else:
        print("\nFailed to connect to Wi-Fi")
        return False


def get_local_version():
    try:
        with open(LOCAL_VERSION_FILE, "r") as f:
            return f.read().strip()
    except:
        return "0.0.0"


def save_local_version(version):
    with open(LOCAL_VERSION_FILE, "w") as f:
        f.write(version)


def check_for_update():
    if not connect_wifi():
        print("No Wi-Fi, skipping OTA check")
        return

    local_ver = get_local_version()
    print("Local version:", local_ver)

    try:
        print("Checking remote version...")
        response = urequests.get(VERSION_FILE_REMOTE)
        remote_ver = response.text.strip()
        response.close()
        print("Remote version:", remote_ver)
    except Exception as e:
        print("Failed to check remote version:", e)
        return

    if remote_ver == local_ver:
        print("Already up to date!")
        return

    print("New version available! {} -> {}".format(local_ver, remote_ver))
    print("Downloading new main.py...")

    try:
        response = urequests.get(MAIN_FILE_REMOTE)
        new_code = response.text
        response.close()

        with open("main.py", "w") as f:
            f.write(new_code)

        save_local_version(remote_ver)
        print("Update complete! Restarting...")
        time.sleep(1)
        machine.reset()

    except Exception as e:
        print("Failed to download update:", e)
