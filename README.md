## Threat Detection System
An ESP32-based motion detection system that triggers a buzzer, LED indicators, and sends an email alert when motion is detected.


## Components Used
- ESP32 Dev Board
- PIR Motion Sensor
- Passive Buzzer
- Red LED (motion detected)
- Green LED (no motion)
- Jumper wires & Breadboard
- Resistors (for LEDs)

## Features
- Real-time motion detection
- Visual alert using LEDs
- Sound alert using passive buzzer
- Email alert with timestamp
- Wi-Fi enabled (ESP32)


## Diagrams of the project
  **schematic diagram**
![image](https://github.com/user-attachments/assets/905cc50f-db50-41a1-82ec-f606b88b1e8c)

 **circuit diagram**
 ![image](https://github.com/user-attachments/assets/ab115a84-812f-45b8-a763-9a818cf59540)


## How It Works
- PIR sensor detects motion
- ESP32 turns ON buzzer & red LED
- Sends email alert with date & time
- Green LED turns ON when no motion
- Cooldown period avoids spam alerts

## Code Setup
- Wi-Fi credentials are stored in WIFI_SSID and WIFI_PASSWORD
- Use Gmail SMTP server for sending emails
- Requires ESP-Mail-Client library

## Library Requirements
Install these libraries from Arduino Library Manager:
- WiFi.h
- ESP_Mail_Client.h

## Setup Instructions
- Connect all components as per the circuit
- Upload the code using Arduino IDE
- Open Serial Monitor (115200 baud)
- Trigger motion to test system
