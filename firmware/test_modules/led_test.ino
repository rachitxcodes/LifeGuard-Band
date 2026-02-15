#define YELLOW_LED 2
#define RED_LED 4

void setup() {
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  digitalWrite(YELLOW_LED, HIGH);
  delay(500);
  digitalWrite(YELLOW_LED, LOW);

  digitalWrite(RED_LED, HIGH);
  delay(500);
  digitalWrite(RED_LED, LOW);
}