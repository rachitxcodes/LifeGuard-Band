# 🔄 Workflow

1. ESP32 continuously monitors MPU6050.
2. If acceleration + rotation exceed threshold → WARNING state.
3. Cancel window allows user reset.
4. If not cancelled → EMERGENCY state.
5. GPS location fetched.
6. SMS sent via GSM.
7. "EMERGENCY" printed over Serial.
8. Python recorder captures video + audio.
9. Data saved locally (can be extended to cloud).
