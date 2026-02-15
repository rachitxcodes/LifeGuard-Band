# 🧠 System Architecture

## Overview

LifeGuard Band is a hybrid embedded + software system.

### Hardware Layer
- Motion sensing (MPU6050)
- GPS location tracking
- GSM communication
- User interface (LED, buzzer, button)

### Firmware Layer (ESP32)
- Impact detection logic
- Emergency state management
- GPS parsing
- SMS alert sending
- Serial trigger for recording

### Software Layer (Python)
- Listens for "EMERGENCY" signal
- Records video + audio
- Saves locally
- Can be extended for cloud upload

## Emergency Flow

Impact Detected  
→ Warning State  
→ Countdown  
→ Emergency Confirmed  
→ GPS Fetch  
→ GSM SMS Sent  
→ Python Recording Triggered
