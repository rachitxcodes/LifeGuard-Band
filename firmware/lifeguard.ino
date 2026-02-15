#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <TinyGPS++.h>

/* ================= WIFI ================= */
const char* ssid = "LifeGuard_Band";
const char* password = "12345678";
WebServer server(80);

/* ================= PINS ================= */
#define YELLOW_LED 2
#define RED_LED    4
#define BUZZER     5
#define BUTTON     13

#define MPU_ADDR 0x68
#define SDA_PIN 21
#define SCL_PIN 22

#define GSM_TX 17
#define GSM_RX 16
#define GPS_TX 26
#define GPS_RX 27

/* ================= GSM ================= */
HardwareSerial gsmSerial(2);
String EMERGENCY_NUMBER = "+91XXXXXXXXXX";

/* ================= GPS ================= */
HardwareSerial gpsSerial(1);
TinyGPSPlus gps;
float LATITUDE = 0.0;
float LONGITUDE = 0.0;

/* ================= STATES ================= */
enum State { READY, WARNING, EMERGENCY };
State currentState = READY;

/* ================= TIMING ================= */
const unsigned long COUNTDOWN_TIME = 7000;
const unsigned long IMPACT_COOLDOWN = 3000;
const unsigned long IMPACT_MIN_DURATION = 120;

unsigned long warningStartTime = 0;
unsigned long lastImpactTime = 0;
unsigned long impactStartTime = 0;
unsigned long ledTimer = 0;
unsigned long buzzerTimer = 0;

/* ================= MPU ================= */
int16_t ax, ay, az, gx, gy, gz;
const float IMPACT_THRESHOLD = 1.2;
const float ROTATION_THRESHOLD = 20.0;

bool impactCandidate = false;
bool emergencySent = false;

/* ================= HELPERS ================= */
void stopAll() {
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

String getMapLink() {
  if (gps.location.isValid()) {
    LATITUDE = gps.location.lat();
    LONGITUDE = gps.location.lng();
    return "https://maps.google.com/?q=" +
           String(LATITUDE, 6) + "," +
           String(LONGITUDE, 6);
  }
  return "Location unavailable";
}

/* ================= GSM SMS ================= */
void sendSMS(String message) {
  gsmSerial.println("AT+CMGF=1");
  delay(1000);
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(EMERGENCY_NUMBER);
  gsmSerial.println("\"");
  delay(1000);
  gsmSerial.println(message);
  delay(100);
  gsmSerial.write(26);
}

/* ================= WEB DASHBOARD ================= */
String dashboardHTML() {
  String color =
    (currentState == READY) ? "#00ff00" :
    (currentState == WARNING) ? "#ffcc00" :
    "#ff0000";

  String mapLink = getMapLink();

  return "<html><body style='background:#111;color:white;text-align:center;'>"
         "<h1>LifeGuard Band</h1>"
         "<h2 style='color:" + color + "'>" + 
         (currentState==READY?"READY":
          currentState==WARNING?"WARNING":"EMERGENCY") +
         "</h2>"
         "<p><a href='" + mapLink + "' target='_blank'>Open Map</a></p>"
         "<a href='/reset'><button>Reset</button></a>"
         "</body></html>";
}

void handleRoot() { server.send(200, "text/html", dashboardHTML()); }
void handleReset() {
  stopAll();
  currentState = READY;
  emergencySent = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

/* ================= SETUP ================= */
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

  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  gsmSerial.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX);

  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  server.begin();
}

/* ================= LOOP ================= */
void loop() {
  server.handleClient();

  while (gpsSerial.available())
    gps.encode(gpsSerial.read());

  unsigned long now = millis();

  /* MPU READ */
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
    }
  } else {
    impactCandidate = false;
  }

  if (currentState == WARNING) {
    digitalWrite(YELLOW_LED, now % 400 < 200);
    digitalWrite(BUZZER, now % 600 < 300);

    if (now - warningStartTime >= COUNTDOWN_TIME) {
      stopAll();
      currentState = EMERGENCY;
    }
  }

  if (currentState == EMERGENCY) {
    if (!emergencySent) {
      String mapLink = getMapLink();
      sendSMS("EMERGENCY ALERT!\nLocation:\n" + mapLink);
      Serial.println("EMERGENCY");
      emergencySent = true;
    }

    digitalWrite(RED_LED, now % 200 < 100);
    digitalWrite(BUZZER, now % 300 < 150);
  }

  if (digitalRead(BUTTON) == LOW) {
    stopAll();
    currentState = READY;
    emergencySent = false;
  }
}
