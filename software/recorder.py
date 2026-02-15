import serial
import subprocess
import time
from datetime import datetime
import os
import sys

# ================= CONFIG =================
SERIAL_PORT = "COM13"          # Change if needed
BAUD_RATE = 115200

RECORD_DURATION = 15           # seconds
COOLDOWN_TIME = 30             # seconds between triggers

VIDEO_DEVICE = "Integrated Camera"
AUDIO_DEVICE = "Microphone Array (2- Realtek(R) Audio)"

SAVE_DIR = "recordings"
# ==========================================


# Create recordings directory if not exists
os.makedirs(SAVE_DIR, exist_ok=True)


def connect_serial():
    """Connect to ESP32 serial port safely."""
    try:
        print(f"🔌 Connecting to ESP32 on {SERIAL_PORT}...")
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)
        print("✅ Serial connected.\n")
        return ser
    except Exception as e:
        print("❌ Failed to connect to ESP32:", e)
        sys.exit(1)


def record_event():
    """Record video + audio using FFmpeg."""
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

    raw_file = os.path.join(
        SAVE_DIR, f"lifeguard_raw_{timestamp}.mp4"
    )

    final_file = os.path.join(
        SAVE_DIR, f"lifeguard_event_{timestamp}.mp4"
    )

    print("🎥 Recording video + audio...")

    record_cmd = [
        "ffmpeg",
        "-y",
        "-rtbufsize", "100M",
        "-f", "dshow",
        "-i", f"video={VIDEO_DEVICE}:audio={AUDIO_DEVICE}",
        "-t", str(RECORD_DURATION),
        "-c:v", "libx264",
        "-c:a", "aac",
        raw_file
    ]

    subprocess.run(record_cmd,
                   stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL)

    print("🔧 Finalizing file...")

    fix_cmd = [
        "ffmpeg",
        "-y",
        "-i", raw_file,
        "-c:v", "libx264",
        "-pix_fmt", "yuv420p",
        "-profile:v", "baseline",
        "-c:a", "aac",
        "-movflags", "+faststart",
        final_file
    ]

    subprocess.run(fix_cmd,
                   stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL)

    # Delete raw file
    if os.path.exists(raw_file):
        os.remove(raw_file)

    print(f"✅ Saved: {final_file}\n")


def main():
    ser = connect_serial()
    print("🟢 Waiting for EMERGENCY trigger...\n")

    last_record_time = 0

    while True:
        try:
            if ser.in_waiting:
                line = ser.readline().decode(errors="ignore").strip()

                if line:
                    print("📩 ESP32:", line)

                if line == "EMERGENCY":
                    now = time.time()

                    # Cooldown protection
                    if now - last_record_time < COOLDOWN_TIME:
                        print("⏳ Cooldown active — ignoring trigger\n")
                        continue

                    last_record_time = now

                    print("🚨 EMERGENCY detected!")
                    record_event()

        except serial.SerialException:
            print("⚠ Serial disconnected. Reconnecting...")
            time.sleep(3)
            ser = connect_serial()

        except KeyboardInterrupt:
            print("\n❌ Stopped by user")
            break

        except Exception as e:
            print("⚠ Error:", e)


if __name__ == "__main__":
    main()