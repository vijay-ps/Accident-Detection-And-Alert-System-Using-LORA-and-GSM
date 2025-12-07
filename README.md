# 🚗 Accident Detection & LoRa Emergency Alert System  
### ESP32-S2 • MPU6050 • GPS Neo-6M • LoRa SX1278 (433 MHz)

A real-time **Accident Detection & Alert System** designed using ESP32-S2, MPU6050, GPS, and LoRa.  
Detects jerks, turns, falls, and crashes — then broadcasts alerts to **all LoRa receivers in range**.

---

## 📌 System Overview (ASCII Diagram)

    ┌─────────────────────────────────────────┐
    │              VEHICLE UNIT               │
    │               (SENDER)                  │
    ├─────────────────────────────────────────┤
    │                                         │
    │   MPU6050 → Motion / Impact Detection   │
    │   GPS Neo-6M → Location Coordinates     │
    │   ESP32-S2 → Processing & Web Config    │
    │   LoRa SX1278 → Broadcast Accident Msg  │
    │                                         │
    └─────────────────────────────────────────┘
                        │
                        │ LoRa Broadcast (433 MHz)
                        ▼
    ┌─────────────────────────────────────────┐
    │               RECEIVER UNIT             │
    │             (ANY LoRa DEVICE)           │
    ├─────────────────────────────────────────┤
    │ Displays data, Alerts, Optional Buzzer  │
    └─────────────────────────────────────────┘

---

## ✨ Features

- 🚦 **5-Level Real-Time Accident Detection**
- 📡 **LoRa Broadcast Mode** (any receiver can read)
- 🛰 **GPS Google Maps Location** auto-attached
- 🌐 **WiFi AP Webpage** to set Car Number
- 📑 **JSON Powered Message Format**
- 🔁 Debounce & false-positive filtering

---

## 🧭 5-Level Detection Table

| Level | Condition | Meaning |
|-------|-----------|---------|
| 1️⃣ | Slight Jerk | Small disturbance |
| 2️⃣ | Speed Breaker | Moderate bump |
| 3️⃣ | Sharp Turn | Hard turn detection |
| 4️⃣ | Fall / Rollover | Risky tilt / vehicle fall |
| 5️⃣ | 🚨 Accident / Crash | Severe impact → Broadcast Trigger |

---

## 🔧 Hardware Components

- **ESP32-S2 DevKitM-1** (processing + WiFi AP)  
- **MPU6050** IMU (acceleration + gyro)  
- **GPS Neo-6M**  
- **LoRa SX1278 433 MHz**  
- (Optional) **SIM800L** for SMS extension

---

## 📡 LoRa Broadcast Message (JSON Format)

    {
      "car": "TN22AB1234",
      "level": 5,
      "type": "ACCIDENT",
      "acc": 29.41,
      "loc": "https://maps.google.com/?q=12.8423,80.1566"
    }

---

## 🌐 WiFi Configuration Page (ASCII Flow)

    User Phone/Laptop
            │
    Connect to WiFi:
    SSID: ESP32-CarSetup
    PASS: 12345678
            │
            ▼
    Open http://192.168.4.1
            │
            ▼
    Enter Car Number → SAVE

---

## 📁 Project Structure

    📦 AccidentDetection-LoRa
    │
    ├── sender/
    │   └── sender.ino        # ESP32-S2 full logic (MPU + GPS + LoRa + WebUI)
    │
    ├── receiver/
    │   └── receiver.ino      # LoRa receiver script
    │
    └── README.md             # Project Documentation

---

## ▶️ Sender Overview (ESP32-S2 Logic Flow)

    Start
      │
      ├── Load saved car number from Preferences
      ├── Initialize MPU6050
      ├── Initialize GPS
      ├── Initialize LoRa
      ├── Start WiFi AP (for configuration)
      │
    Loop:
      ├── Read sensors (MPU)
      ├── Calculate motion magnitude
      ├── Classify into Levels 1–5
      ├── If Level 5 & cooldown passed:
      │      ├── Get GPS / fallback location
      │      ├── Build JSON message
      │      └── Broadcast over LoRa
      │
      └── Listen for LoRa messages (debug)

---

## ▶️ Receiver Overview

    Start LoRa Receiver
            │
    Listen for Packets
            │
            ▼
    If Accident Packet:
       Print data
       Show RSSI
       Optionally trigger buzzer/LED

---

## 🛠 Installation & Flashing

### Sender (ESP32-S2)
1. Open Arduino IDE  
2. Select board → **ESP32S2 Dev Module**  
3. Install libraries:  
   - Adafruit MPU6050  
   - TinyGPS++  
   - LoRa (Sandeep Mistry)  
4. Flash `sender.ino`

### Receiver
- Flash `receiver.ino` into any LoRa-capable board

---

## 🚀 Tuning & Testing

1. Calibrate thresholds during stationary test.  
2. Start with lower sensitivity, monitor false positives.  
3. Test speed breaker, turns, simulated fall, and crash cases.  
4. Tune threshold constants in `sender.ino` accordingly.  
5. Test LoRa broadcast range and RSSI on receiver.

---

## 🚀 Future Extensions

- 📞 SIM800L SMS Alerting  
- ☁️ Cloud dashboard (MQTT + WebSocket + Maps)  
- 🔐 AES-encrypted LoRa packets  
- 📱 Android/iOS app for notifications  
- 📊 Crash analytics & logging (SD card or cloud)

---

## ❤️ Credits
Made with ❤️ by **PS Vijay** using ESP32 + LoRa + Embedded Systems.

---

