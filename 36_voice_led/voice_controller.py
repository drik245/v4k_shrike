import serial
import speech_recognition as sr
import time
import sys

# ---------------------------------------------------------
# SETUP
# ---------------------------------------------------------
# UPDATE THIS TO MATCH YOUR BOARD'S COM PORT IN ARDUINO IDE!
# E.g., 'COM3' on Windows, '/dev/ttyACM0' on Linux/Mac
COM_PORT = 'COM3' 
BAUD_RATE = 115200

print("Initializing Serial Connection...")
try:
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    time.sleep(2) # Give the board 2 seconds to reset after opening port
    print(f"Successfully connected to {COM_PORT}\n")
except serial.SerialException:
    print(f"ERROR: Could not open {COM_PORT}.")
    print("Please check your Arduino IDE to see which COM port your board is connected to, and update the COM_PORT variable in this script.")
    sys.exit(1)

# Initialize Speech Recognizer
recognizer = sr.Recognizer()

# ---------------------------------------------------------
# MAIN LOOP
# ---------------------------------------------------------
def listen_and_control():
    with sr.Microphone() as source:
        print("Calibrating for ambient noise... Please wait 1 second.")
        recognizer.adjust_for_ambient_noise(source, duration=1)
        print("\n--- READY ---")
        print("Say something like 'Turn the light ON' or 'Turn it OFF'.")
        print("Press Ctrl+C to quit.\n")
        
        while True:
            try:
                # Listen to the microphone
                audio = recognizer.listen(source, timeout=5, phrase_time_limit=4)
                
                # Send audio to Google's free speech recognition API
                text = recognizer.recognize_google(audio).lower()
                print(f"You said: '{text}'")
                
                # Parse commands
                if "on" in text:
                    print("--> Sending ON command to board...")
                    ser.write(b"ON\n")
                elif "off" in text:
                    print("--> Sending OFF command to board...")
                    ser.write(b"OFF\n")
                    
            except sr.WaitTimeoutError:
                # No speech detected within timeout, just loop back
                pass
            except sr.UnknownValueError:
                print("(Could not understand audio, please speak clearer)")
            except sr.RequestError as e:
                print(f"Could not request results from Google API; {e}")
            except KeyboardInterrupt:
                print("\nExiting...")
                break

if __name__ == "__main__":
    listen_and_control()
    ser.close()
