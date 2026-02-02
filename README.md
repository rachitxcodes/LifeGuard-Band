# 🛡️ LifeGuard Band

LifeGuard Band is a wearable IoT-based emergency response system designed to automatically detect accidents and send location-based alerts without relying on smartphones or internet connectivity.

## 🚨 Problem Statement
In emergency situations such as accidents or physical threats, victims are often unable to use their phones. This leads to delayed response and increased risk to life.

## 💡 Solution
LifeGuard Band continuously monitors motion using an accelerometer and allows manual SOS triggering. When an emergency is detected, it sends an SMS alert with GPS location to predefined emergency contacts using GSM.

## ⚙️ Core Features
- Automatic accident / fall detection (MPU6050)
- Manual SOS button
- False alert reduction with cancel window
- GPS-based real-time location sharing
- GSM-based SMS alerts (works without internet)
- Battery-powered wearable design

## 🧠 System Architecture
- ESP32 as central controller
- MPU6050 for motion sensing
- GPS for location
- GSM module for emergency alerts
- LED & buzzer for user feedback

## 🔌 Hardware Components
- ESP32
- MPU6050
- GSM Module (2G)
- GPS Module
- SOS Button
- Buzzer & LED
- Li-ion Battery + TP4056

## 📁 Repository Structure
- firmware/ – ESP32 code
- hardware/ – circuit & wiring
- docs/ – architecture & workflow
- media/ – demo visuals

## 🚀 Future Enhancements
- Audio evidence recording
- Cloud dashboard using Wi-Fi
- AI-based motion analysis
- Health sensors integration

## 👨‍💻 Team
LifeGuard Band – Hackathon Project
