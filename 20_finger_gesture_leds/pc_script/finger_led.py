import cv2
from cvzone.HandTrackingModule import HandDetector
import serial
import time
import sys

# Configure this to match your Shrike board's COM port!
COM_PORT = 'COM16'
BAUD_RATE = 115200

print(f"Connecting to {COM_PORT} at {BAUD_RATE} baud...")
try:
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)
    print("Connected successfully!")
except Exception as e:
    print(f"Failed to connect to {COM_PORT}: {e}")
    print("Please update the COM_PORT variable in this script and try again.")
    sys.exit(1)

# Initialize hand detector (max 1 hand to keep it simple)
detector = HandDetector(detectionCon=0.8, maxHands=1)
cap = cv2.VideoCapture(0)

last_count = -1

while True:
    success, img = cap.read()
    if not success:
        print("Failed to grab webcam frame")
        break
        
    # Flip image so it acts like a mirror
    img = cv2.flip(img, 1)
    
    # Detect hands
    hands, img = detector.findHands(img, draw=True)
    
    # Count fingers
    current_count = 0
    if hands:
        hand = hands[0]
        # fingersUp() returns a list of 5 ints [thumb, index, middle, ring, pinky] (1=up, 0=down)
        fingers = detector.fingersUp(hand)
        current_count = sum(fingers)
        
    # Only send serial command if the finger count has changed
    if current_count != last_count:
        cmd = f"LEDS:{current_count}\n"
        ser.write(cmd.encode())
        print(f"Sent: {cmd.strip()}")
        last_count = current_count
        
    # Display the finger count on the video feed
    cv2.putText(img, f"Fingers: {current_count}", (20, 70), cv2.FONT_HERSHEY_PLAIN, 3, (0, 255, 0), 3)
    cv2.imshow("Finger Gesture Controlled LEDs", img)
    
    # Press 'q' to quit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Clean up
cap.release()
cv2.destroyAllWindows()
ser.write(b"LEDS:0\n") # Turn off LEDs on exit
ser.close()
