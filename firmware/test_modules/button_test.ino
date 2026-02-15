#define BUTTON 13

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    Serial.println("Button Pressed");
    delay(500);
  }
}