#define GSM_TX 17
#define GSM_RX 16

HardwareSerial gsmSerial(2);

String PHONE_NUMBER = "+91XXXXXXXXXX";

void setup() {
  Serial.begin(115200);
  gsmSerial.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX);

  delay(3000);

  gsmSerial.println("AT");
  delay(1000);

  gsmSerial.println("AT+CMGF=1");
  delay(1000);

  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(PHONE_NUMBER);
  gsmSerial.println("\"");
  delay(1000);

  gsmSerial.println("Test SMS from LifeGuard Band");
  delay(100);

  gsmSerial.write(26);

  Serial.println("SMS Sent");
}

void loop() {}