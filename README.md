# 🛡️ LifeGuard Band  
### Detect • Protect • Respond

LifeGuard Band is a wearable IoT-based emergency response system designed to automatically detect accidents and trigger real-time location-based alerts — without relying on smartphones or internet connectivity.

Built for rapid emergency response in critical situations.

---

## 📌 Overview

In emergency situations such as road accidents, physical threats, or sudden falls, victims are often unable to access their phones. This delay in communication can significantly increase response time and risk to life.

LifeGuard Band solves this problem by functioning as a **self-contained emergency alert system** that operates independently using GSM and GPS modules.

---

## 🚨 Problem Statement

- Victims cannot always unlock or access phones during emergencies  
- Internet connectivity may not be available  
- Delayed alerts reduce survival probability  
- Manual emergency reporting is unreliable under stress  

---

## 💡 Proposed Solution

LifeGuard Band continuously monitors motion patterns using an accelerometer (MPU6050).

When:
- A sudden fall or abnormal motion is detected  
- OR the SOS button is pressed manually  

The system:
1. Activates buzzer + LED warning  
2. Provides a short cancellation window to avoid false alerts  
3. Fetches GPS coordinates  
4. Sends SMS alert with real-time location via GSM  

All of this works **without internet connectivity**.

---

## ⚙️ Core Features

- ✅ Automatic accident / fall detection  
- ✅ Manual SOS emergency trigger  
- ✅ False alert reduction system  
- ✅ GPS-based live location sharing  
- ✅ GSM-based SMS alerts (No Internet Required)  
- ✅ Battery-powered wearable architecture  
- ✅ Real-time user feedback via LED & buzzer  

---

## 🧠 System Architecture

**Controller:** ESP32  
**Motion Sensor:** MPU6050  
**Location Module:** GPS  
**Communication Module:** GSM (2G)  
**Alert Interface:** LED + Buzzer  
**Power System:** Li-ion Battery + TP4056 Charging Module  

**System Flow:**

Motion Detection → Validation → Cancel Window → GPS Fetch → GSM SMS → Alert Sent

---

## 🔌 Hardware Components

| Component | Purpose |
|-----------|----------|
| ESP32 | Central microcontroller |
| MPU6050 | Motion & fall detection |
| GSM Module | SMS alert transmission |
| GPS Module | Real-time location |
| SOS Button | Manual emergency trigger |
| Buzzer & LED | Alert indication |
| Li-ion Battery | Portable power source |
| TP4056 | Battery charging module |

---

## 📁 Repository Structure

```
LifeGuard-Band/
│
├── firmware/
│   └── lifeguard_core/
│       └── lifeguard_core.ino     # Main ESP32 program
│
├── test_modules/
│   ├── button_test.ino
│   ├── buzzer_test.ino
│   ├── gps_test.ino
│   ├── gsm_test.ino
│   ├── led_test.ino
│   └── mpu_test.ino               # Individual hardware test codes
│
├── hardware/
│   ├── components_list.md
│   └── pin_connections.md         # Circuit documentation
│
├── docs/
│   ├── system_architecture.md
│   ├── workflow.md
│   └── cost_analysis.md
│
├── software/
│   ├── recorder.py
│   └── recordings/
│
├── README.md
└── .gitignore

```

---

## 🎯 Target Use Cases

- Road accident victims  
- Women safety wearable  
- Elderly fall detection  
- Industrial worker safety  
- Remote area emergency alert system  

---

## 🚀 Future Enhancements

- 🎙 Audio evidence recording system  
- ☁ Wi-Fi cloud dashboard for monitoring  
- 🤖 AI-based motion pattern classification  
- ❤️ Health monitoring sensors integration  
- 📱 Mobile companion app  

---

## 🏆 Project Type

Hackathon Innovation Project  
IoT + Embedded Systems + Emergency Response  

---

## 👨‍💻 Team

Developed as part of the **LifeGuard Band Hackathon Project**

---
