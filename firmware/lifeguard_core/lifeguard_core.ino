#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>

/* ================== WIFI ================== */
const char* ssid = "LifeGuard_Band";
const char* password = "12345678";
WebServer server(80);

/* ================== PIN DEFINITIONS ================== */
#define YELLOW_LED 2
#define RED_LED    4
#define BUZZER     5
#define BUTTON     13

#define MPU_ADDR 0x68
#define SDA_PIN 21
#define SCL_PIN 22

/* ================== DUMMY GPS (MANUAL) ================== */
float LATITUDE  = 27.604948;
float LONGITUDE = 77.595330;

/* ================== SYSTEM STATES ================== */
enum State { READY, WARNING, EMERGENCY, CANCELLED };
State currentState = READY;

/* ================== TIMING ================== */
const unsigned long COUNTDOWN_TIME = 7000;
const unsigned long IMPACT_COOLDOWN = 3000;
const unsigned long IMPACT_MIN_DURATION = 120;

unsigned long warningStartTime = 0;
unsigned long lastImpactTime = 0;
unsigned long impactStartTime = 0;

/* ================== BLINK TIMERS ================== */
unsigned long ledTimer = 0;
unsigned long buzzerTimer = 0;
bool ledState = false;
bool buzzerState = false;

/* ================== MPU DATA ================== */
int16_t ax, ay, az, gx, gy, gz;

/* ================== TUNING ================== */
const float IMPACT_THRESHOLD = 1.2;
const float ROTATION_THRESHOLD = 20.0;

/* ================== FLAGS ================== */
bool impactCandidate = false;
bool emergencySent = false;

/* ================== HELPERS ================== */
String stateToString() {
  if (currentState == READY) return "READY";
  if (currentState == WARNING) return "WARNING";
  if (currentState == EMERGENCY) return "EMERGENCY";
  return "CANCELLED";
}

void stopAll() {
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

/* ================== WEB DASHBOARD ================== */
String dashboardHTML() {
  String color =
    (currentState == READY) ? "#00ff00" :
    (currentState == WARNING) ? "#ffcc00" :
    "#ff0000";

  String mapLink = "https://maps.google.com/?q=" +
                   String(LATITUDE, 6) + "," +
                   String(LONGITUDE, 6);

  String locationBlock;

  if (currentState == EMERGENCY) {
    locationBlock =
      "<p><b>Latitude:</b> " + String(LATITUDE, 6) + "</p>"
      "<p><b>Longitude:</b> " + String(LONGITUDE, 6) + "</p>"
      "<p><a href='" + mapLink + "' target='_blank'>📍 Open in Google Maps</a></p>";
  } else {
    locationBlock =
      "<p style='color:#888;'>Waiting for GPS fix…</p>";
  }

  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv="refresh" content="1">
<style>
body {
  background:#111;
  color:white;
  font-family:Arial;
  text-align:center;
}
.card {
  margin:20px;
  padding:20px;
  border-radius:12px;
  border:3px solid )rawliteral" + color + R"rawliteral(;
}
button {
  padding:15px 30px;
  font-size:18px;
  border:none;
  border-radius:10px;
  background:#ff4444;
  color:white;
}
a { color:#4da6ff; }
</style>
</head>
<body>

<h1> LifeGuard Band</h1>

<div class="card">
<h2>Status</h2>
<h1 style="color:)rawliteral" + color + R"rawliteral("> )rawliteral" +
stateToString() + R"rawliteral( </h1>
</div>

<div class="card">
<h2> Location (GPS)</h2>
)rawliteral" + locationBlock + R"rawliteral(
</div>

<div class="card">
<a href="/reset"><button> Cancel / False Alert</button></a>
</div>

</body>
</html>
)rawliteral";
}
/* ================== WEB ROUTES ================== */
void handleRoot() {
  server.send(200, "text/html", dashboardHTML());
}

void handleReset() {
  stopAll();
  currentState = READY;
  emergencySent = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

/* ================== SETUP ================== */
void setup() {
  Serial.begin(115200);

  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  server.begin();

  Serial.println("READY");
}

/* ================== LOOP ================== */
void loop() {
  server.handleClient();
  unsigned long now = millis();

  /* ---- READ MPU ---- */
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14);

  ax = Wire.read()<<8 | Wire.read();
  ay = Wire.read()<<8 | Wire.read();
  az = Wire.read()<<8 | Wire.read();
  Wire.read(); Wire.read();
  gx = Wire.read()<<8 | Wire.read();
  gy = Wire.read()<<8 | Wire.read();
  gz = Wire.read()<<8 | Wire.read();

  float Ax = ax / 16384.0;
  float Ay = ay / 16384.0;
  float Az = az / 16384.0;

  float Gx = gx / 131.0;
  float Gy = gy / 131.0;
  float Gz = gz / 131.0;

  float accMag = sqrt(Ax*Ax + Ay*Ay + Az*Az);
  bool highRotation = abs(Gx)>ROTATION_THRESHOLD ||
                      abs(Gy)>ROTATION_THRESHOLD ||
                      abs(Gz)>ROTATION_THRESHOLD;

  /* ---- IMPACT DETECTION ---- */
  if (currentState == READY &&
      accMag > IMPACT_THRESHOLD &&
      highRotation &&
      now - lastImpactTime > IMPACT_COOLDOWN) {

    if (!impactCandidate) {
      impactCandidate = true;
      impactStartTime = now;
    }

    if (now - impactStartTime >= IMPACT_MIN_DURATION) {
      impactCandidate = false;
      lastImpactTime = now;
      currentState = WARNING;
      warningStartTime = now;
      Serial.println("IMPACT DETECTED");
    }
  } else {
    impactCandidate = false;
  }

  /* ---- WARNING BLINK ---- */
  if (currentState == WARNING) {
    if (now - ledTimer > 400) {
      ledState = !ledState;
      digitalWrite(YELLOW_LED, ledState);
      ledTimer = now;
    }
    if (now - buzzerTimer > 600) {
      buzzerState = !buzzerState;
      digitalWrite(BUZZER, buzzerState);
      buzzerTimer = now;
    }
    if (now - warningStartTime >= COUNTDOWN_TIME) {
      stopAll();
      currentState = EMERGENCY;
    }
  }

  /* ---- EMERGENCY ---- */
  if (currentState == EMERGENCY) {
    if (!emergencySent) {
      Serial.println("EMERGENCY");
      emergencySent = true;
    }
    if (now - ledTimer > 200) {
      ledState = !ledState;
      digitalWrite(RED_LED, ledState);
      ledTimer = now;
    }
    digitalWrite(BUZZER, millis() % 300 < 150);
  }

  /* ---- BUTTON RESET ---- */
  if (digitalRead(BUTTON) == LOW) {
    stopAll();
    currentState = READY;
    emergencySent = false;
  }
}
